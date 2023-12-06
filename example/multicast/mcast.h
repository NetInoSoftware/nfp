#ifndef _MCAST_H_
#define _MCAST_H_

#include <odp_api.h>

#define APP_PORT 2048
#define APP_ADDR_MCAST "234.5.5.5"

int nfp_multicast_thread(nfp_thread_t *thread_mcast, int core_id);

#endif
