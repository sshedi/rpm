#include "system.h"

#include <filesystem>
#include <string>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <popt.h>
#include <ctype.h>

#include <rpm/rpmfileutil.h>
#include <rpm/rpmurl.h>
#include <rpm/rpmmacro.h>
#include <rpm/rpmlog.h>
#include <rpm/argv.h>

#include "rpmio_internal.hh"
#include "rpmmacro_internal.hh"

#include "debug.h"

namespace fs = std::filesystem;

int rpmDoDigest(int algo, const char * fn,int asAscii, unsigned char * digest)
{
    unsigned char * dig = NULL;
    size_t diglen, buflen = 32 * BUFSIZ;
    std::vector<uint8_t> buf(buflen);
    int rc = 0;

    FD_t fd = Fopen(fn, "r.ufdio");

    if (fd) {
	fdInitDigest(fd, algo, 0);
	while ((rc = Fread(buf.data(), 1, buflen, fd)) > 0) {};
	fdFiniDigest(fd, algo, (void **)&dig, &diglen, asAscii);
    }

    if (dig == NULL || Ferror(fd)) {
	rc = 1;
    } else {
	memcpy(digest, dig, diglen);
    }

    dig = _free(dig);
    Fclose(fd);

    return rc;
}

FD_t rpmMkTemp(char *templ)
{
    mode_t mode;
    int sfd;
    FD_t tfd = NULL;

    mode = umask(0077);
    sfd = mkstemp(templ);
    umask(mode);

    if (sfd < 0) {
	goto exit;
    }

    tfd = fdDup(sfd);
    close(sfd);

exit:
    return tfd;
}

FD_t rpmMkTempFile(const char * prefix, char **fn)
{
    const char *tpmacro = "%{_tmppath}"; /* always set from rpmrc */
    char *tempfn;
    static int _initialized = 0;
    FD_t tfd = NULL;

    if (!prefix) prefix = "";

    /* Create the temp directory if it doesn't already exist. */
    if (!_initialized) {
	_initialized = 1;
	tempfn = rpmGenPath(prefix, tpmacro, NULL);
	if (rpmioMkpath(tempfn, 0755, (uid_t) -1, (gid_t) -1))
	    goto exit;
	free(tempfn);
    }

    tempfn = rpmGetPath(prefix, tpmacro, "/rpm-tmp.XXXXXX", NULL);
    tfd = rpmMkTemp(tempfn);

    if (tfd == NULL || Ferror(tfd)) {
	rpmlog(RPMLOG_ERR, _("error creating temporary file %s: %m\n"), tempfn);
	goto exit;
    }

exit:
    if (tfd != NULL && fn)
	*fn = tempfn;
    else
	free(tempfn);

    return tfd;
}

int rpmioMkpath(const char * path, mode_t mode, uid_t uid, gid_t gid)
{
    char *d, *de;
    int rc;

    if (path == NULL || *path == '\0')
	return -1;
    d = rstrcat(NULL, path);
    if (d[strlen(d)-1] != '/') {
	rstrcat(&d,"/");
    }
    de = d;
    for (;(de=strchr(de+1,'/'));) {
	struct stat st;
	*de = '\0';
	rc = mkdir(d, mode);
	if (rc && errno == EEXIST) {
	    rc = stat(d, &st);
	    if (rc)
		goto exit;
	    if (!S_ISDIR(st.st_mode)) {
		rc = ENOTDIR;
		goto exit;
	    }
	} else if (rc) {
	    goto exit;
	} else {
	    rpmlog(RPMLOG_DEBUG, "created directory(s) %s mode 0%o\n", path, mode);
	    if (!(uid == (uid_t) -1 && gid == (gid_t) -1)) {
		rc = chown(d, uid, gid);
		if (rc)
		    goto exit;
	    }
	}
	*de = '/';
    }
    rc = 0;
exit:
    free(d);
    return rc;
}

int rpmFileIsCompressed(const char * file, rpmCompressedMagic * compressed)
{
    FD_t fd;
    ssize_t nb;
    int rc = -1;
    unsigned char magic[13];

    *compressed = COMPRESSED_NOT;

    fd = Fopen(file, "r.ufdio");
    if (fd == NULL || Ferror(fd)) {
	/* XXX Fstrerror */
	rpmlog(RPMLOG_ERR, _("File %s: %s\n"), file, Fstrerror(fd));
	if (fd) (void) Fclose(fd);
	return 1;
    }
    nb = Fread(magic, sizeof(magic[0]), sizeof(magic), fd);
    if (nb < 0) {
	rpmlog(RPMLOG_ERR, _("File %s: %s\n"), file, Fstrerror(fd));
	rc = 1;
    } else if (nb < sizeof(magic)) {
	rpmlog(RPMLOG_ERR, _("File %s is smaller than %u bytes\n"),
		file, (unsigned)sizeof(magic));
	rc = 0;
    }
    (void) Fclose(fd);
    if (rc >= 0)
	return rc;

    rc = 0;

    if ((magic[0] == 'B') && (magic[1] == 'Z') &&
        (magic[2] == 'h')) {
	*compressed = COMPRESSED_BZIP2;
    } else if ((magic[0] == 'P') && (magic[1] == 'K') &&
	 (((magic[2] == 3) && (magic[3] == 4)) ||
	  ((magic[2] == '0') && (magic[3] == '0')))) {	/* pkzip */
	*compressed = COMPRESSED_ZIP;
    } else if ((magic[0] == 0xfd) && (magic[1] == 0x37) &&
	       (magic[2] == 0x7a) && (magic[3] == 0x58) &&
	       (magic[4] == 0x5a) && (magic[5] == 0x00)) {
	/* new style xz (lzma) with magic */
	*compressed = COMPRESSED_XZ;
    } else if ((magic[0] == 0x28) && (magic[1] == 0xB5) &&
	       (magic[2] == 0x2f)                     ) {
	*compressed = COMPRESSED_ZSTD;
    } else if ((magic[0] == 'L') && (magic[1] == 'Z') &&
	       (magic[2] == 'I') && (magic[3] == 'P')) {
	*compressed = COMPRESSED_LZIP;
    } else if ((magic[0] == 'L') && (magic[1] == 'R') &&
	       (magic[2] == 'Z') && (magic[3] == 'I')) {
	*compressed = COMPRESSED_LRZIP;
    } else if (((magic[0] == 0037) && (magic[1] == 0213)) || /* gzip */
	((magic[0] == 0037) && (magic[1] == 0236)) ||	/* old gzip */
	((magic[0] == 0037) && (magic[1] == 0036)) ||	/* pack */
	((magic[0] == 0037) && (magic[1] == 0240)) ||	/* SCO lzh */
	((magic[0] == 0037) && (magic[1] == 0235))	/* compress */
	) {
	*compressed = COMPRESSED_OTHER;
    } else if ((magic[0] == '7') && (magic[1] == 'z') &&
               (magic[2] == 0xbc) && (magic[3] == 0xaf) &&
               (magic[4] == 0x27) && (magic[5] == 0x1c)) {
	*compressed = COMPRESSED_7ZIP;
    } else if (rpmFileHasSuffix(file, ".lzma")) {
	*compressed = COMPRESSED_LZMA;
    } else if (rpmFileHasSuffix(file, ".gem")) {
	*compressed = COMPRESSED_GEM;
    }

    return rc;
}

/* @todo "../sbin/./../bin/" not correct. */
char *rpmCleanPath(char * path)
{
    const char *s;
    char *se, *t, *te;
    int begin = 1;

    if (path == NULL)
	return NULL;

    s = t = te = path;
    while (*s != '\0') {
	switch (*s) {
	case ':':			/* handle url's */
	    if (s[1] == '/' && s[2] == '/') {
		*t++ = *s++;
		*t++ = *s++;
		break;
	    }
	    begin=1;
	    break;
	case '/':
	    /* Move parent dir forward */
	    for (se = te + 1; se < t && *se != '/'; se++)
		{};
	    if (se < t && *se == '/') {
		te = se;
	    }
	    while (s[1] == '/')
		s++;
	    while (t > path && t[-1] == '/')
		t--;
	    break;
	case '.':
	    /* Leading .. is special */
 	    /* Check that it is ../, so that we don't interpret */
	    /* ..?(i.e. "...") or ..* (i.e. "..bogus") as "..". */
	    /* in the case of "...", this ends up being processed*/
	    /* as "../.", and the last '.' is stripped.  This   */
	    /* would not be correct processing.                 */
	    if (begin && s[1] == '.' && (s[2] == '/' || s[2] == '\0')) {
		*t++ = *s++;
		break;
	    }
	    /* Single . is special */
	    if (begin && s[1] == '\0') {
		break;
	    }
	    /* Handle the ./ cases */
	    if (t > path && t[-1] == '/') {
		/* Trim embedded ./ */
		if (s[1] == '/') {
		    s+=2;
		    continue;
		}
		/* Trim trailing /. */
		if (s[1] == '\0') {
		    s++;
		    continue;
		}
	    }
	    /* Trim embedded /../ and trailing /.. */
	    if (!begin && t > path && t[-1] == '/' && s[1] == '.' && (s[2] == '/' || s[2] == '\0')) {
		t = te;
		/* Move parent dir forward */
		if (te > path)
		    for (--te; te > path && *te != '/'; te--)
			{};
		s++;
		s++;
		continue;
	    }
	    break;
	default:
	    begin = 0;
	    break;
	}
	*t++ = *s++;
    }

    /* Trim trailing / (but leave single / alone) */
    if (t > &path[1] && t[-1] == '/')
	t--;
    *t = '\0';

    return path;
}

/* Merge 3 args into path */

char * rpmGenPath(const char * urlroot, const char * urlmdir,
		const char *urlfile)
{
    std::string path = rpm::join_path({ urlroot ? urlroot : "",
					urlmdir ? urlmdir : "",
					urlfile ? urlfile : "" });
    return xstrdup(path.c_str());
}

/* Return concatenated and expanded canonical path. */

char * rpmGetPath(const char *path, ...)
{

    if (path == NULL)
	return xstrdup("");

    std::string dest;
    va_list ap;
    va_start(ap, path);
    for (const char *s = path; s; s = va_arg(ap, const char *))
	dest += s;
    va_end(ap);

    auto p = rpm::expand_path({dest});
    return xstrdup(p.c_str());
}

static char * rpmEscapeChars(const char *s, const char *accept, int (*fn)(int))
{
    const char * se;
    char * t;
    char * te;
    size_t nb = 0;

    for (se = s; *se; se++) {
	if ((accept && strchr(accept, *se)) || (fn && fn(*se)))
	    nb++;
	nb++;
    }
    nb++;

    t = te = (char *)xmalloc(nb);
    for (se = s; *se; se++) {
	if ((accept && strchr(accept, *se)) || (fn && fn(*se)))
	    *te++ = '\\';
	*te++ = *se;
    }
    *te = '\0';
    return t;
}

char * rpmEscapeSpaces(const char *s)
{
    return rpmEscapeChars(s, NULL, isspace);
}

char * rpmEscape(const char *s, const char *accept)
{
    return rpmEscapeChars(s, accept, NULL);
}

void rpmUnescape(char *s, const char *accept)
{
    char *p, *q;
    int skip = 0;

    p = q = s;
    while (*q != '\0') {
	*p = *q++;
	if (*p == '\\' && (!accept || strchr(accept, *q)) && !skip) {
	    skip = 1;
	    continue;
	}
	p++;
	skip = 0;
    }
    *p = '\0';
}

int rpmFileHasSuffix(const char *path, const char *suffix)
{
    size_t plen = strlen(path);
    size_t slen = strlen(suffix);
    return (plen >= slen && rstreq(path+plen-slen, suffix));
}

char * rpmGetCwd(void)
{
    std::error_code ec;
    auto currDir = fs::current_path(ec);
    return ec ? NULL : xstrdup(currDir.c_str());;
}

int rpmMkdirs(const char *root, const char *pathstr)
{
    ARGV_t dirs = NULL;
    int rc = 0;
    argvSplit(&dirs, pathstr, ":");
    
    for (char **d = dirs; *d; d++) {
	char *path = rpmGetPath(root ? root : "", *d, NULL);
	if (strstr(path, "%{"))
	    rpmlog(RPMLOG_WARNING, ("undefined macro(s) in %s: %s\n"), *d, path);
	if ((rc = rpmioMkpath(path, 0755, -1, -1)) != 0) {
	    const char *msg = _("failed to create directory");
	    /* try to be more informative if the failing part was a macro */
	    if (**d == '%') {
	    	rpmlog(RPMLOG_ERR, "%s %s: %s: %m\n", msg, *d, path);
	    } else {
	    	rpmlog(RPMLOG_ERR, "%s %s: %m\n", msg, path);
	    }
	}
	free(path);
	if (rc) break;
    }
    argvFree(dirs);
    return rc;
}

/* One-shot initialization of our global config directory */
struct rpmConfDir {
    std::string path;
    rpmConfDir() {
	char *rpmenv = getenv("RPM_CONFIGDIR");
	path = rpmenv ? rpmenv : RPM_CONFIGDIR;
    };
};

const char *rpmConfigDir(void)
{
    static rpmConfDir confDir {};
    return confDir.path.c_str();
}

std::string
rpm::join_path(const std::initializer_list<std::string> & args, bool expand)
{
    std::string path;
    for (auto const & a : args) {
	/* rpmGenPath() can call us with empty arguments */
	if (!a.empty()) {
	    std::string s = expand ? rpm::expand_path({a}) : a;
	    path += s + '/';
	}
    }
    return rpm::normalize_path(path);
}

std::string
rpm::expand_path(const std::initializer_list<std::string> & args)
{
    auto [ rc, s ] = macros().expand(args);
    return rpm::normalize_path(s);
}

std::string
rpm::normalize_path(const std::string & arg)
{
    std::string path = fs::path(arg).lexically_normal();
    if (path.size() > 1 && path.back() == '/')
	path.pop_back();
    return path;
}
