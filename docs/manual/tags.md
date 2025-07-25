---
layout: default
title: rpm.org - RPM Tags
---
# RPM Tags

The package's meta data in stored in the RPM header. The header is a binary data structure that stores the single pieces of data in tags. Each tag has a pre-defined meaning and data type. These are not stored in the header itselfs but need to be known by the code reading the header. In the header the tags are only refered by their number.

## Tag types

### Scalar types

There are four unsigned integer types for RPM tags:

* `int8`
* `int16`
* `int32`
* `int64`

Additionally there is a `char` datatype that is also only used once.

There is a `bin` datatype for arbitrary data that is basically an `char` array.

There are two string types: Plain `string` with is zero terminated and of arbitary length (within the header size restriction).

`i18nstring`s are translated to the requested locale when queried.

### Arrays

Each tag is either of a plain scalar type or is an array of one of these types. While not enforced by the type system the RPM code assumes that tags belonging together have the same number of entries e.g. one for each file.

### Mappings

While not technically a type on its own there are several mappings. These consists of two or more tags working together. One array (called dictionary, often named `*dict`) contains a list of values while another tag (called an index) contains integers referencing a position in the first one. The index can either be a plain integer or an integer array.

## Base package tags

Tag Name    | Value| Type       | Description
------------|------|------------|------------
Name        | 1000 | string     | Package name.
Version     | 1001 | string     | Package version.
Release     | 1002 | string     | Package release.
Epoch       | 1003 | int32      | Package epoch (optional). An absent epoch is equal to epoch value `0`.
License     | 1014 | string     | License of the package contents.
Summary     | 1004 | i18nstring | One line summary of the package's purpose.
Description | 1005 | i18nstring | Multiline description of the package's purpose
Os          | 1021 | string     | The operating system the package is for
Arch        | 1022 | string     | The architecture the package is for. `noarch` is a special case denoting a architecture independent package.

## Informative package tags

Tag Name           | Value| Type         | Description
-------------------|------|--------------|------------
Buildhost          | 1007 | string       | Hostname of the system the package was built on.
Buildtime          | 1006 | int32        | Unix timestamp of package build time.
Bugurl             | 5012 | string       | URL to package bug tracker.
Changelogname      | 1081 | string array | Per entry changelog author information, typically `name <email>`.
Changelogtext      | 1082 | string array | Per entry changelog text.
Changelogtime      | 1080 | int32 array  | Per entry changelog Unix timestamp.
Cookie             | 1094 | string       | An opaque value for tracking packages from a single build operation.
Distribution       | 1010 | string       | Distribution name.
Disttag            | 1155 | string       | Distribution acronym.
Disturl            | 1123 | string       | Distribution specific URL of the package.
Encoding           | 5062 | string       | Encoding of the header string data. When present it is always `utf-8` and the data has actually been validated.
Group              | 1016 | i18nstring   | Group of the package.
Modularitylabel    | 5096 | string
Optflags           | 1122 | string       | `%{optflags}` value at the time of package build.
Packager           | 1015 | string       | Packager contact information.
Platform           | 1132 | string       | Package platform (arch-os-vendor).
Policies           | 1150 | string array
Policyflags        | 5033 | int32 array
Policynames        | 5030 | string array
Policytypes        | 5031 | string array
Policytypesindexes | 5032 | int32 array
Rpmformat          | 5114 | int32        | Rpm package format version
Rpmversion         | 1064 | string       | Version of rpm used to build the package.
Sourcepkgid        | 1146 | bin
Sourcerpm          | 1044 | string       | Package source rpm file name.
Translationurl     | 5100 | string       | URL of upstream translation service/repository
UpstreamReleases   | 5101 | string       | URL to check for newer releases from upstream e.g. for build systems to check for newer upstream releases and then to notify the packager.
Url                | 1020 | string       | Package URL, typically project upstream website.
Vcs                | 5034 | string       | (Public) upstream source code VCS location. Format `<vcs>:<address>` with `<vcs>` being the VCS command used (e.g. `git`, `svn`, `hg`, ...) and `<address>` being the location of the repository as used by the VCS tool to clone/checkout the repository (e.g. `https://github.com/rpm-software-management/rpm.git`).
Vendor             | 1011 | string       | Package vendor contact information.
Sourcenevr         | 5120 | string       | Source RPM NEVR

## Packages with files

Tag Name          | Value| Type         | Description
------------------|------|--------------|------------
Archivesize       | 1046 | int32        | (Uncompressed) payload size.
Dirnames          | 1118 | string array | dirname(3) components of contained paths
Filedigestalgo    | 5011 | int32        | ID of file digest algorithm. If missing, considered `0` for `md5`.
Longarchivesize   | 271  | int64        | (Uncompressed) payload size when > 4GB.
Longsize          | 5009 | int64        | Installed package size when > 4GB.
Mimedict          | 5116 | int32        | Dictionary of MIME types, only >= v6.
Payloadcompressor | 1125 | string       | Payload compressor name (see `rpm-payloadflags`(7))
Payloadflags      | 1126 | string       | Payload compressor level (see `rpm-payloadflags`(7))
Payloadformat     | 1124 | string       | Payload format (`cpio`)
Prefixes          | 1098 | string array | Relocatable prefixes (on relocatable packages).
Size              | 1009 | int32        | Installed package size.

## Per-file information

Tag Name        | Value| Type         | Description
----------------|------|--------------|------------
Basenames       | 1117 | string array | basename(3) of the path.
Dirindexes      | 1116 | int32 array  | Index into dirname(3) array of the pacakge (see Dirname tag).
Filedevices     | 1095 | int32 array  | Abstract device ID (hardlink calculation only).
Filedigests     | 1035 | string array | File cryptographic digest (aka hash) using algorithm specified in  Filedigestalgo.
Fileflags       | 1037 | int32 array  | File virtual attributes (doc, license, ghost, artifact etc)
Filegroupname   | 1040 | string array | Unix group name.
Fileinodes      | 1096 | int32 array  | Abstract inode number (hardlink calculation only).
Filelangs       | 1097 | string array | Optional language of the file (eg man page translations)
Filelinktos     | 1036 | string array | Symlink target for symlink files.
Filemimeindex   | 5115 | int32 array  | Index into MIME dictionary (see Mimedict tag), only >= v6.
Filemodes       | 1030 | int16 array  | Unix file mode.
Filemtimes      | 1034 | int32 array  | Unix file modification timestamp (aka mtime).
Filerdevs       | 1033 | int16 array  | Device ID (of device files)
Filesizes       | 1028 | int32 array  | File size (when all files are < 4GB)
Fileusername    | 1039 | string array | Unix owner name
Fileverifyflags | 1045 | int32 array  | File verification flags (which aspects of file to verify)
Longfilesizes   | 5008 | int64 array  | File size (when files > 4GB are present)

### Optional file information

Tag Name            | Value| Type         | Description
--------------------|------|--------------|------------
Classdict           | 1142 | string array | File class (libmagic string) dictionary (only v4)
Dependsdict         | 1145 | int32 array  | File dependencies dictionary
Filecaps            | 5010 | string array | `cap_to_text(3)` textual representation of file capabilities.
Fileclass           | 1141 | int32 array  | Index into Classdict (only v4)
Filecolors          | 1140 | int32 array  | File "color" - 1 for 32bit ELF, 2 for 64bit ELF and 0 otherwise
Filedependsn        | 1144 | int32 array  | Number of file dependencies in Dependsdict, starting from Filedependsx
Filedependsx        | 1143 | int32 array  | Index into Dependsdict denoting start of this file's dependencies.
Filesignaturelength | 5091 | int32        | IMA signature length.
Filesignatures      | 5090 | string array | IMA signature (hex encoded).
Veritysignaturealgo | 277  | int32        | fsverity signature algorithm ID.
Veritysignatures    | 276  | string array | fsverity signature (base64 encoded).

## Dependency information

All dependency tags follow the same pattern. The name tag contains
dependency token names, the flags tag may contain additional context
information and the dependency is versioned as per (`RPMSENSE_*` bitfield),
the range related to the version specified in the version tag.

### Hard dependencies

Tag Name       | Value| Type
---------------|------|--------------
Providename    | 1047 | string array
Provideversion | 1113 | string array
Provideflags   | 1112 | int32 array

Requirename    | 1049 | string array
Requireversion | 1050 | string array
Requireflags   | 1048 | int32 array

Conflictname    | 1054 | string array
Conflictversion | 1055 | string array
Conflictflags   | 1053 | int32 array

Obsoletename    | 1090 | string array
Obsoleteversion | 1115 | string array
Obsoleteflags   | 1114 | int32 array

### Soft dependencies

Tag Name       | Value| Type
---------------|------|--------------
Enhancename    | 5055 | string array
Enhanceversion | 5056 | string array
Enhanceflags   | 5057 | int32 array

Recommendname    | 5046 | string array
Recommendversion | 5047 | string array
Recommendflags   | 5048 | int32 array

Suggestname    | 5049 | string array
Suggestversion | 5050 | string array
Suggestflags   | 5051 | int32 array

Supplementname    | 5052 | string array
Supplementversion | 5053 | string array
Supplementflags   | 5054 | int32 array

Ordername    | 5035 | string array
Orderversion | 5036 | string array
Orderflags   | 5037 | int32 array

## Scriptlets

### Basic scriptlets

All scriptlet tags follow the same pattern. The main tag contains the
corresponding scriptlet (eg `%post`) body. If present, the prog tag
denotes the scriptlet interpreter and possible arguments, and flags tag
contains additional processing information such as whether to macro
expand the scriptlet body.

`%post` script is executed right after the package got installed

Tag Name    | Value| Type
------------|------|--------------
Postin      | 1024 | string
Postinflags | 5021 | int32
Postinprog  | 1086 | string array

`%posttrans` scripts are all executed at the end of the transaction that installed their packages

Posttrans      | 1152 | string
Posttransflags | 5025 | int32
Posttransprog  | 1154 | string array

`%postuntrans` scripts are all executed at the end of the transaction that removes their packages

Postuntrans      | 5104 | string
Postuntransflags | 5108 | int32
Postuntransprog  | 5106 | string array

`%postun` script is executed right after the package was removed

Postun      | 1026 | string
Postunflags | 5023 | int32
Postunprog  | 1088 | string array

`%pre` script is executed right before the package is installed

Prein      | 1023 | string
Preinflags | 5020 | int32
Preinprog  | 1085 | string array

`%pretrans` scripts are executed for to be installed packages before any packages are installed/removed

Pretrans      | 1151 | string
Pretransflags | 5024 | int32
Pretransprog  | 1153 | string array

`%preuntrans` scripts are executed for to be removed packages before any packages are installed/removed

Preuntrans      | 5103 | string
Preuntransflags | 5107 | int32
Preuntransprog  | 5105 | string array

`%preun` script is executed right before the package gets removed

Preun      | 1025 | string
Preunflags | 5022 | int32
Preunprog  | 1087 | string array

`%verify` script is executed when the package is verified (e.g. with `rpm -V`)

Verifyscript      | 1079 | string
Verifyscriptflags | 5026 | int32
Verifyscriptprog  | 1091 | string array

### Triggers

[Triggers](triggers.md) are stored in a combination of dependency and scriptlet tags.

Triggerscripts, Triggerscriptflags and Triggerscriptprog form a scriptlet
triplet, and Triggername, Triggerflags, Triggerversion form a dependency
triplet. Triggerindexes is a per dependency index into Triggerscripts,
binding the two triplets together.

Tag Name           | Value| Type
-------------------|------|--------------
Triggerflags       | 1068 | int32 array
Triggerindex       | 1069 | int32 array
Triggername        | 1066 | string array
Triggerscriptflags | 5027 | int32 array
Triggerscriptprog  | 1092 | string array
Triggerscripts     | 1065 | string array
Triggerversion     | 1067 | string array

### File triggers

[File trigger](file_triggers.md) tags are like normal trigger tags,
with an additional priority tag to affect trigger running order. File
triggers in Filetrigger-tags run once per triggered package, whereas
triggers in Transfiletrigger-tags run once per transaction.

Tag Name               | Value| Type
-----------------------|------|--------------
Filetriggerflags       | 5072 | int32 array
Filetriggerindex       | 5070 | int32 array
Filetriggername        | 5069 | string array
Filetriggerpriorities  | 5084 | int32 array
Filetriggerscriptflags | 5068 | int32 array
Filetriggerscriptprog  | 5067 | string array
Filetriggerscripts     | 5066 | string array
Filetriggerversion     | 5071 | string array

Transfiletriggerflags       | 5082 | int32 array
Transfiletriggerindex       | 5080 | int32 array
Transfiletriggername        | 5079 | string array
Transfiletriggerpriorities  | 5085 | int32 array
Transfiletriggerscriptflags | 5078 | int32 array
Transfiletriggerscriptprog  | 5077 | string array
Transfiletriggerscripts     | 5076 | string array
Transfiletriggerversion     | 5081 | string array

## Signatures and digests

[Signatures](signatures.md) allow to verify the origin of a package.

Tag Name          | Value| Type         | Description
------------------|------|--------------|------------
Dsaheader         | 267  | bin          | OpenPGP DSA signature of the header (if thus signed)
Longsigsize       | 270  | int64        | Header+payload size if > 4GB.
Openpgp           | 278  | string array | OpenPGP signature(s) of the header, base64 encoded
Payloadsha256     | 5092 | string array | SHA256 digest of the compressed payload.
Payloadsha256algo | 5093 | int32        | ID of the SHA256 algorithm (obsolete)
Payloadsha256alt  | 5097 | string array | SHA256 digest of the uncompressed payload.
Payloadsha3_256   | 5123 | string       | SHA3-256 digest of the compressed payload.
Payloadsha3_256alt| 5124 | string       | SHA3-256 digest of the uncompressed payload.
Rsaheader         | 268  | bin          | OpenPGP RSA signature of the header (if thus signed).
Sha1header        | 269  | string       | SHA1 digest of the header.
Sha256header      | 273  | string       | SHA256 digest of the header.
Sha3_256_header   | 279  | string       | SHA3-256 digest of the header.
Siggpg            | 262  | bin          | OpenPGP DSA signature of the header+payload (if thus signed).
Sigmd5            | 261  | bin          | MD5 digest of the header+payload.
Sigpgp            | 259  | bin          | OpenPGP RSA signature of the header+payload (if thus signed).
Sigsize           | 257  | int32        | Header+payload size.

## Installed package headers only

The following tags are added to the headers during installation and do not
exist in RPM package files.


Tag Name       | Value| Type         | Description
---------------|------|--------------|------------
Filestates     | 1029 | char array   | Per-file installed status information (installed/skipped/forced etc)
Installcolor   | 1127 | int32        | "Color" of transaction in which the package was installed.
Installtid     | 1128 | int32        | ID of transaction in which the package was installed.
Installtime    | 1008 | int32        | Unix timestamp of package installation.
Instprefixes   | 1099 | string array
Origbasenames  | 1120 | string array | Original Basenames (relocated packages only)
Origdirindexes | 1119 | int32 array  | Original Dirindexes (relocated packages only)
Origdirnames   | 1121 | string array | Original Dirnames (relocated packages only)
Packagedigests | 5118 | string array | Package digests calculated during verification
Packagedigestalgos | 5119 | int32    | Algorithms used for Packagedigests


## Source packages

Tag Name      | Value| Type         | Description
--------------|------|--------------|------------
Buildarchs    | 1089 | string array | If present, specifies the architectures the package may be built for.
Excludearch   | 1059 | string array | If present, limits the architectures on which the package is buildable by excluding those specified.
Excludeos     | 1060 | string array | If present, limits the operating systems on which the package is buildable by excluding those specified.
Exclusivearch | 1061 | string array | If present, limits the architectures on which the package is buildable exclusively to those specified.
Exclusiveos   | 1062 | string array | If present, limits the operating systems on which the package is buildable exclusively to those specified.
Nopatch       | 1052 | int32 array  | Denotes a patch number for which source is not supplied.
Nosource      | 1051 | int32 array  | Denotes a source number for which source is not supplied.
Patch         | 1019 | string array | Patch file names.
Source        | 1018 | string array | Source file names.
Sourcepackage | 1106 | int32        | Denotes a source rpm.
Spec          | 5099 | string       | Expanded and parsed spec contents.

## Internal / special

Tag Name        | Value| Type         | Description
----------------|------|--------------|------------
Headeri18ntable | 100 | string array | Locales for which the header has translations.
Headerimmutable | 63  | bin          | Special tag to return the unmodified, original image of the header even after data has been added to it in eg installation.
Pubkeys         | 266  | string array | used in gpg-pubkey special packages in the RPM data base

## Deprecated / Obsolete

These tags are not longer in active use. If encountered in packages
they are ignored.

Tag Name           | Value| Type         | Description
-------------------|------|--------------|------------
Filecontexts       | 1147 | string array
Fscontexts         | 1148 | string array
Gif                | 1012 | bin
Icon               | 1043 | bin
Oldenhancesname    | 1159 | string array
Oldenhancesversion | 1160 | string array
Oldenhancesflags   | 1161 | int32 array
Oldfilenames       | 1027 | string array
Oldsuggestsname    | 1156 | string array
Oldsuggestsversion | 1157 | string array
Oldsuggestsflags   | 1158 | int32 array
Patchesflags       | 1134 | int32 array
Patchesname        | 1133 | string array
Patchesversion     | 1135 | string array
Recontexts         | 1149 | string array
Removetid          | 1129 | int32
Xpm                | 1013 | bin

## Aliases

Aliases are simply shorthands or alternative names for other tags.

Alias Name  | Tag             | Type         | Description
------------|-----------------|--------------|------------
C           | Conflictname    | string array
Conflicts   | Conflictname    | string array
E           | Epoch           | int32
Enhances    | Enhancename     | string array
Filemd5s    | Filedigests     | string array
Hdrid       | Sha1header      | string
N           | Name            | string
Obsoletes   | Obsoletename    | string array
O           | Obsoletename    | string array
Oldenhances | Oldenhancesname | string array
Oldsuggests | Oldsuggestsname | string array
P           | Providename     | string array
Pkgid       | Sigmd5          | bin
Provides    | Providename     | string array
R           | Release         | string
Recommends  | Recommendname   | string array
Requires    | Requirename     | string array
Suggests    | Suggestname     | string array
Supplements | Supplementname  | string array
V           | Version         | string

## Extensions

Extension tags do not exist in concrete form anywhere, their data is
constructed at query time based on other information in the headers
or runtime environment. A few physical tags do have an extension format
as well however, these exceptions noted below.

Tag Name      | Value| Type         | Description
--------------|------|--------------|------------
Archsuffix    | 5098 | string       | Package file arch suffix (".src", ".nosrc" or .`arch`)
Dbinstance    | 1195 | int32        | Header ID of installed package, 0 otherwise.
Epochnum      | 5019 | int32        | Package epoch as numeric value (0 if not present).
Evr           | 5013 | string       | Formatted `epoch:version-release` string of the package
Fileclass     | 1141 | string array | Per-file formatted libmagic classification strings, calculated from the Fileclass and Classdict tags. Note overlap with the concrete Fileclass tag with different type (integer array)!
Headercolor   | 5017 | int32        | Header "color" calculated from file colors.
Nevr          | 5015 | string       | Formatted `name-epoch:version-release` string of the package
Nevra         | 5016 | string       | Formatted `name-epoch:version-release.arch` string of the package
Nvr           | 5014 | string       | Formatted `name-version-release` string of the package
Nvra          | 1196 | string       | Formatted `name-version-release.arch` string of the package
Filenames     | 5000 | string array | Per file paths contained in the package, calculated from the path triplet.
Filenlinks    | 5045 | int32 array  | Per file hardlink number, calculated from inode/device information.
Fileprovide   | 5001 | string array | Per file dependency capabilities provided by the corresponding files.
Filerequire   | 5002 | string array | Per file dependency capabilities required by the corresponding files.
Instfilenames | 5040 | string array | Per file paths installed from the package, calculated from the path triplet and file status info.


These tags provide 64bit values regardless of underlying storage size. They return the value of the concrete tag if present or the value of the corresponding  32bit tag as an 64bit value. Always use these to access the sizes. See [Large File support](large_files.md) for details.

Longfilesizes | Per file sizes in 64bit format
Longarchivesize | Archive size in 64bit format
Longsize | Uncompressed size in 64bit format
Longsigsize | Header+payload size in 64bit format

Tag Name              | Value| Type         | Description
----------------------|------|--------------|------------
Openpgp               |  278 | string array | All OpenPGP signature(s) in the header, including legacy ones (base64 encoded)
Origfilenames         | 5007 | string array | Original Filenames in relocated packages.
Providenevrs          | 5042 | string array | Formatted `name [op version]` provide dependency strings.
Conflictnevrs         | 5044 | string array | Formatted `name [op version]` conflict dependency strings.
Obsoletenevrs         | 5043 | string array | Formatted `name [op version]` obsolete dependency strings.
Enhancenevrs          | 5061 | string array | Formatted `name [op version]` enhance dependency strings.
Recommendnevrs        | 5058 | string array | Formatted `name [op version]` recommend dependency strings.
Requirenevrs          | 5041 | string array | Formatted `name [op version]` require dependency strings.
Rpmformat             | 5114 | int32        | Detected rpm format version (3/4/6)
Suggestnevrs          | 5059 | string array | Formatted `name [op version]` suggest dependency strings.
Supplementnevrs       | 5060 | string array | Formatted `name [op version]` supplement dependency strings.
Sysusers              | 5109 | string array | Formatted systemd-sysusers lines for the package. |
Filetriggerconds      | 5086 | string array | Formatted file trigger condition information.
Filetriggertype       | 5087 | string array | Formatted file trigger type information.
Transfiletriggerconds | 5088 | string array | Formatted transaction file trigger condition information.
Transfiletriggertype  | 5089 | string array | Formatted transaction file trigger type information.
Triggerconds          | 5005 | string array | Formatted trigger condition information.
Triggertype           | 5006 | string array | Formatted trigger type information.
Verbose               | 5018 | int32        | Returns 1 if rpm is running in verbose mode.
