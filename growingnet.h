#ifndef GROWINGNET_H
#define GROWINGNET_H

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

typedef unsigned long UL;

EXTERNC UL& init();

EXTERNC const UL growingnet = init();

#undef EXTERNC
#endif
