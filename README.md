Netino Fast Path is a spin-off of OpenFastPath project.
===============================================================================

Netino Fast Path is an effort to provide the overall improvement of the
functionality offered by OpenFastPath project while exploring its utilization
in diffent scenarios like small devices or multi-stack setups.

It does not target the API or design compatibility with OpenFastPath project.

Netino Fast Path (NFP) project web page:<br>
    http://www.netinosoft.org

NFP project in GitHub:<br>
    https://github.com/NetInoSoftware/nfp

Netino Fast Path general info
===============================================================================


Intent and purpose:
-------------------------------------------------------------------------------
The intent of this project is to enable accelerated routing/forwarding for
IPv4 and IPv6, tunneling and termination for a variety of protocols.
Unsupported functionality is provided by the host OS networking stack
(slow path).

Netino Fast Path functionality is provided as a library to Fast Path applications
that use ODP run to completion execution model and framework. DPDK is supported
through the ODP-DPDK layer.

Termination of protocols with POSIX-like interface (socket) for legacy
applications is also supported.

See [project technical overview](http://openfastpath.org/index.php/services/technical-overview/)
for more details about architecture and main features.


Directory structure
-------------------------------------------------------------------------------
./config/      - Example configuration files.<br>
./docs/        - This is where you can find more detailed documentation.<br>
./example/     - Example applications that use the NFP libraries.<br>
./include/api/ - Public interface headers used by an application.<br>
./lib/         - NFP libraries.<br>
./scripts/     - Helper scripts.<br>
./test/        - Sanity test applications.<br>


Coding Style:
-------------------------------------------------------------------------------
Project code uses Linux kernel style that is verified through `checkpatch.pl`


Licensing:
-------------------------------------------------------------------------------
Project uses BSD 3-CLause License as default license. One should not use code
that is licensed under any GPL type.


NFP getting started
===============================================================================


Build environment preparation:
-------------------------------------------------------------------------------
This project is currently verified on a generic 64bit x86 Linux machine.

The following packages (+ dependencies) are mandatory for building the NFP applications:

    git autotools-dev libtool automake build-essential pkg-config

The following packages are optional:

    libssl-dev doxygen asciidoc valgrind libcunit1 libcunit1-doc libcunit1-dev libconfig-dev

The usage of libconfig-dev package is enabled by default and can be disabled by --disable-libconfig
configure option.

Additionally, the following packages may be needed (e.g. with DPDK): libnuma-dev,
libpcap-dev and python.

OpenDataPlane (ODP) is a mandatory library. It can be either manually built
from the sources or built with the devbuild script.

Build ODP library (manually):
-------------------------------------------------------------------------------
Download (clone) and build ODP:

    git clone https://github.com/OpenDataPlane/odp-dpdk
    cd odp-dpdk
    git checkout v1.41.0.0_DPDK_22.11
    ./bootstrap
    ./configure --prefix=<INSTALL ODP TO THIS DIR> --with-platform=linux-generic
    make
    make install

(`make install` may require root permissions)

Instructions for building NFP on top of ODP-DPDK with DPDK interface type
support can be found on NFP User Guide (`docs/nfp-user-guide.adoc`).

Build NFP applications (manually):
-------------------------------------------------------------------------------
Download (clone) and build NFP:

    git clone https://github.com/NetInoSoftware/nfp
    cd nfp
    ./bootstrap
    ./configure --prefix=<INSTALL NFP TO THIS DIR> --with-odp=<ODP INSTALLATION DIR>
    make
    make install 

(`make install` may require root permissions)

Build NFP applications, odp-dpdk and dpdk (devbuild script)
-------------------------------------------------------------------------------
The 'devbuild' script builds from sources DPDK, ODP-DPDK and the NFP examples
applications:

    git clone https://github.com/NetInoSoftware/nfp
    cd nfp
    ./scripts/devbuild_all.sh

'nfp/scripts/devbuild_all/install' is the install folder of the build.

NFP example applications:
-------------------------------------------------------------------------------
NFP project contains a number of example applications described in
`example/README` file. See NFP User Guide (`docs/nfp-user-guide.adoc`) for
more details about designing and executing NFP applications.

NFP library version
===============================================================================
NFP library version is 8.1.0 Ocicat

ODP/DPDK recommended versions:
===============================================================================

NFP supports a wider variety of ODP and DPDK versions but recommended
(tested) versions are:
 - ODP-DPDK (https://github.com/OpenDataPlane/odp-dpdk) version v1.41.0.0,
 platforms 'linux-generic' and 'linux-dpdk'.
 - DPDK version v22.11.3.

