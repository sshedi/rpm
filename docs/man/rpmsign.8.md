---
date: 'Red Hat, Inc'
section: 8
title: RPMSIGN
---

NAME
====

rpmsign - RPM Package Signing

SYNOPSIS
========

SIGNING PACKAGES:
-----------------

**rpmsign** **\--addsign\|\--resign** \[**rpmsign-options**\] *PACKAGE\_FILE
\...*

**rpmsign** **\--delsign** *PACKAGE\_FILE \...*

**rpmsign** **\--delfilesign** *PACKAGE\_FILE \...*

rpmsign-options
---------------

\[**\--rpmv3**\] \[**\--rpmv4**\] \[**\--fskpath** *KEY*\] \[**\--signfiles**\]

DESCRIPTION
===========

**rpmsign** **\--addsign** generates and inserts a new OpenPGP signature
for each *PACKAGE\_FILE* given unless a signature with identical
parameters already exists, in which case no action is taken.
Arbitrary number of V6 signatures can be added.

**rpmsign** **\--resign** generates and inserts a new OpenPGP signature
for each *PACKAGE\_FILE*, replacing any and all previous signatures.

To create a signature rpmsign needs to verify the package\'s checksum. As a
result V4 packages with MD5/SHA1 checksums cannot be signed in FIPS mode.

**rpmsign** **\--delsign** *PACKAGE\_FILE \...*

Delete all OpenPGP signatures from each package *PACKAGE\_FILE* given.

**rpmsign** **\--delfilesign** *PACKAGE\_FILE \...*

Delete all IMA and fsverity file signatures from each package
*PACKAGE\_FILE* given.

SIGN OPTIONS
------------

**\--rpmv3**

:   Request RPM V3 header+payload signature addition on V4 packages.
    These signatures are expensive
    and redundant baggage on packages where a separate payload digest
    exists (packages built with rpm \>= 4.14). Rpmsign will automatically
    detect the need for V3 signatures, but this option can be used to
    request their creation if the packages must be fully signature
    verifiable with rpm \< 4.14 or other interoperability reasons.

    Has no effect when signing V6 packages.

**\--rpmv4**

:   Request RPM V4 header signature addition on V6 packages.
    Useful for making V6 packages signature verifiable
    with rpm 4.x versions.

    V4 compatibility signatures are only ever added if the signing algorithm
    is one of those known to V4: RSA, EcDSA, EdDSA (and original DSA).
    Only one V4 signature can be present in a package, so this is
    added only on the first **\--addsign** with a V4 compatible
    algorithm, and ignored otherwise.

    Has no effect when signing V4 packages.

**\--rpmv6**

:   Request RPM V6 header signature addition on V4 packages.

    This generally always succeeds as there can be arbitrary number of
    V6 signatures on a package. A V3/V4 compatibility signatures are
    added usign the same logic as **\--rpmv4** on a V6 package.

    Has no effect when signing V6 packages.

**\--fskpath** *KEY*

:   Used with **\--signfiles**, use file signing key *KEY*.

**\--certpath** *CERT*

:   Used with **\--signverity**, use file signing certificate *CERT*.

**\--verityalgo** *ALG*

:   Used with **\--signverity**, to specify the signing algorithm.
    sha256 and sha512 are supported, with sha256 being the default if
    this argument is not specified. This can also be specified with the
    macro *%\_verity\_algorithm*.

**\--signfiles**

:   Sign package files. The macro **%\_binary\_filedigest\_algorithm**
    must be set to a supported algorithm before building the package.
    The supported algorithms are SHA1, SHA256, SHA384, and SHA512, which
    are represented as 2, 8, 9, and 10 respectively. The file signing
    key (RSA private key) must be set before signing the package, it can
    be configured on the command line with **\--fskpath** or the macro
    %\_file\_signing\_key.

**\--signverity**

:   Sign package files with fsverity signatures. The file signing key
    (RSA private key) and the signing certificate must be set before
    signing the package. The key can be configured on the command line
    with **\--fskpath** or the macro %\_file\_signing\_key, and the cert
    can be configured on the command line with **\--certpath** or the
    macro %\_file\_signing\_cert.


CONFIGURING SIGNING KEYS
------------------------

In order to sign packages, you need to create your own OpenPGP key pair
(aka certificate) and configure **rpm**(8) to use it. The following macros are
available:

**%\_openpgp_sign_id**

:   The fingerprint or keyid of the signing key to use. Typically
    this is the only configuration needed. If omitted,
    **--key-id** must be explicitly specified when signing.

**%\_openpgp_sign**

:   The OpenPGP implementation to use for signing. Supported values are
    \"gpg\" for GnuPG (default and traditional) and \"sq\" for Sequoia PGP.

Implementation specific macros:

**%\_gpg\_path**

:   The location of your GnuPG keyring if not the default **\$GNUPGHOME**.

**%\_gpg\_name**

:   Legacy macro for configuring user id with GnuPG. Use the implementation
    independent and non-ambiguous **%\_openpgp_sign_id** instead.

**%\_sq\_path**

:   The location of your Sequoia configuration if not the default.

For example, to configure rpm to sign with Sequoia PGP using a key with
fingerprint of 7B36C3EE0CCE86EDBC3EFF2685B274E29F798E08 you would include

%_openpgp_sign sq
%_openpgp_signer 7B36C3EE0CCE86EDBC3EFF2685B274E29F798E08

in a macro configuration file, typically ~/.config/rpm/macros.
See **Macro Configuration** in **rpm**(8) for more details.

SEE ALSO
========

**popt**(3), **rpm**(8), **rpmdb**(8), **rpmkeys**(8), **rpm2cpio**(8),
**rpmbuild**(8), **rpmspec**(8)

**rpmsign \--help** - as rpm supports customizing the options via popt
aliases it\'s impossible to guarantee that what\'s described in the
manual matches what\'s available.

**http://www.rpm.org/ \<URL:http://www.rpm.org/\>**
