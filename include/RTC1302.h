#ifndef _RTC_H_
#define _RTC_H_

#include <ds1302.h>


#ifdef __cplusplus
extern "C" {
#endif

extern void RTC_init(void);

void ds1302_sync_time_with_ntp(struct tm *time);

void get_time_from_RTC(struct tm *time);

#ifdef __cplusplus
}
#endif

#endif
