/*
 * Copyright (C) 2019 ASR Micro Limited
 * All Rights Reserved.
 */

#ifndef __JPU_YUV_FEEDER_H__
#define __JPU_YUV_FEEDER_H__

#include "BufferAllocatorWrapper.h"
#include "datastructure.h"
#include "jpuapi.h"

#define SOURCE_YUV 0

typedef void* YuvFeeder;

typedef struct YuvFeederListenerArg {
  Uint32 currentRow; /* Updated by caller */
  Uint32 height;     /* Updated by caller */
  void* context;
} YuvFeederListenerArg;

typedef void (*YuvFeederListener)(YuvFeederListenerArg*);

typedef enum { YUV_FEEDER_MODE_NORMAL, YUV_FEEDER_MODE_THREAD } YuvFeederMode;

typedef struct SliceNotifyParam {
  Uint32 rows;
  BOOL notify;
  void* arg; /* You can access this variable at YuvFeederListernerArg::arg */
} SliceNotifyParam;

typedef enum {
  /* The yuv feeder will notify a client when a slice is filled
   * The rows of a slice must be multiple of 8(400,422,444) or 16(420)
   * Argument is a pointer value that has the number of height of the slice.
   * This command is valid when handle is created with with
   * YUV_FEEDER_MODE_THREAD.
   */
  YUV_FEEDER_CMD_SET_SLICE_NOTIFY,
} YuvFeederCmd;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
  Uint32 width;
  Uint32 height;
  Uint32 bpp;
  BOOL bigEndian;
  BOOL lsbJustification;    /* Little endian basis */
  Uint32 chromaInterleaved; /* 0: planara, 1: semi-planar(NV12, NV16,..), 2:
                               semi-planar(NV21, NV61,...) */
  Uint32 packedFormat;      /* YUV422: YUYV, UYVY, ... */
  FrameFormat format;
} YuvAttr;

typedef void* YuvFeederContext;

struct YuvFeederImpl;

typedef struct {
  struct YuvFeederImpl* impl;
  void* thread;
  BOOL threadStop;
  YuvFeederListener listener;
  Uint32 fbEndian;
  Queue* fbQueue;
  Uint32 sliceHeight;
  void* sliceNotiCtx;
  YuvAttr attr;
} AbstractYuvFeeder;

typedef struct YuvFeederImpl {
  void* context;
  BOOL (*Create)
  (AbstractYuvFeeder* feeder, const char* path, JdiDeviceCtx devctx);
  Int32 (*Feed)(AbstractYuvFeeder* feeder, FrameBufferInfo* fb, Uint32 endian,
                BufferAllocator* bufferAllocator);
  BOOL (*Destroy)(AbstractYuvFeeder* feeder);
  BOOL (*Configure)(AbstractYuvFeeder* feeder, Uint32 cmd, void* arg);
} YuvFeederImpl;

extern YuvFeeder YuvFeeder_Create(YuvFeederMode mode, const char* path,
                                  YuvAttr attr, Uint32 fbEndian,
                                  YuvFeederListener listener,
                                  JdiDeviceCtx devctx);

extern BOOL YuvFeeder_Destroy(YuvFeeder feeder);

extern BOOL YuvFeeder_Feed(YuvFeeder feeder, FrameBufferInfo* fb,
                           BufferAllocator* bufferAllocator);

extern BOOL YuvFeeder_Configure(YuvFeeder feeder, YuvFeederCmd cmd, void* arg);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __JPU_YUV_FEEDER_H__ */
