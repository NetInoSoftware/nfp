#!/bin/bash

export ROOT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

export NFP_NETWRAP_ENV_DEFAULT="-i eth1 -f ${ROOT_DIR}/nfp_netwrap.cli"
export NFP_NETWRAP_ENV="${NFP_NETWRAP_ENV:-${NFP_NETWRAP_ENV_DEFAULT}}"

LD_PRELOAD=libnfp_netwrap_crt.so:libnfp_netwrap_proc.so:libnfp.so $@
