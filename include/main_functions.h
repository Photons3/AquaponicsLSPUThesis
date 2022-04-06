#ifndef _MAIN_FUNCTIONS_H_
#define _MAIN_FUNCTIONS_H_

#ifdef __cplusplus
extern "C" {
#endif

extern void vMainTask(void* params);

extern void vHeater(void *params);

extern void vFishFeed(void *params);

extern void vAerator(void *params);

#ifdef __cplusplus
}
#endif

#endif