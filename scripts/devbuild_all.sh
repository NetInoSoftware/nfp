#!/bin/bash -xe

JOBS=${JOBS:-16}

export ROOT_DIR=$(readlink -e $(dirname $0))
export REPOS="${REPOS:-${ROOT_DIR}/devbuild_all}"

mkdir ${REPOS}
cd ${REPOS}

echo '#include "pcap.h"' | cpp -H -o /dev/null 2>&1 || \
    echo "Warning: pcap is not installed. You may need to install libpcap-dev"

echo '#include "numa.h"' | cpp -H -o /dev/null 2>&1 || \
    echo "Warning: NUMA library is not installed. You need to install libnuma-dev"

git -c advice.detachedHead=false clone -q --depth=1 --branch=20.11 http://dpdk.org/git/dpdk-stable dpdk
pushd dpdk
git log --oneline --decorate

#Configure DPDK
meson build
pushd build
meson configure -Dprefix=`pwd`/../install

#Build DPDK
ninja install
popd
popd

# Clone odp-dpdk
git clone -q https://github.com/OpenDataPlane/odp-dpdk
pushd odp-dpdk
git checkout -b local_v1.41.0.0 v1.41.0.0_DPDK_22.11

echo > $(pwd)/platform/linux-generic/Makefile.inc

export CONFIGURE_FLAGS="--enable-shared=yes --enable-helper-linux"

#Build ODP
./bootstrap
PKG_CONFIG_PATH=`pwd`/../dpdk/install/lib/x86_64-linux-gnu/pkgconfig ./configure  --enable-debug --enable-debug-print \
	     --prefix=$(pwd)/install --with-platform=linux-generic \
	     --enable-dpdk --enable-dpdk-zero-copy
make -j${JOBS} install
popd

cd ${ROOT_DIR}/..
./bootstrap
./configure --with-odp=$REPOS/odp-dpdk/install --prefix=$REPOS/install
make -j${JOBS} install
