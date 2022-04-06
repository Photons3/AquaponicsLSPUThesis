#ifndef _WIFI_CONNECTION_H_
#define _WIFI_CONNECTION_H_

#include <ds1302.h>

#ifdef __cplusplus
extern "C" {
#endif

void obtain_time(void);

void wifi_init(void);

#ifdef __cplusplus
}
#endif

#endif
