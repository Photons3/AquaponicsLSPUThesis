#ifndef _WIFI_CONNECTION_H_
#define _WIFI_CONNECTION_H_

#include <ds1302.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void obtain_time(void);

extern void wifi_init_sta(void);

#ifdef __cplusplus
}
#endif

#endif
