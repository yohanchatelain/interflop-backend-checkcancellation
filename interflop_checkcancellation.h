
/*--------------------------------------------------------------------*/
/*--- Verrou: a FPU instrumentation tool.                          ---*/
/*--- Interface for floatin-point operations overloading           ---*/
/*--- designed to detect cancellation                              ---*/
/*---                               interflop_checkcancellation_.h ---*/
/*--------------------------------------------------------------------*/

/*
   This file is part of Verrou, a FPU instrumentation tool.

   Copyright (C) 2014-2021 EDF
     F. Févotte     <francois.fevotte@edf.fr>
     B. Lathuilière <bruno.lathuiliere@edf.fr>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307, USA.

   The GNU Lesser General Public License is contained in the file COPYING.
*/

#ifndef __INTERFLOP_CHECKCANCELLATION_H__
#define __INTERFLOP_CHECKCANCELLATION_H__

//#define DEBUG_PRINT_OP

#ifdef __cplusplus
extern "C" {
#endif
#define INTERFLOP_CHECKCANCELLATION_API(FCT) interflop_checkcancellation_##FCT

#include "interflop-stdlib/interflop.h"
#include "interflop-stdlib/interflop_stdlib.h"

typedef struct checkcancellation_conf {
  unsigned int threshold_b32;
  unsigned int threshold_b64;
} checkcancellation_conf_t;

typedef checkcancellation_conf_t checkcancellation_context_t;

void INTERFLOP_CHECKCANCELLATION_API(configure)(checkcancellation_conf_t mode,
                                                void *context);
void INTERFLOP_CHECKCANCELLATION_API(finalize)(void *context);

const char *INTERFLOP_CHECKCANCELLATION_API(get_backend_name)(void);
const char *INTERFLOP_CHECKCANCELLATION_API(get_backend_version)(void);

void INTERFLOP_CHECKCANCELLATION_API(pre_init)(File *stream,
                                               interflop_panic_t panic,
                                               void **context);

struct interflop_backend_interface_t
    INTERFLOP_CHECKCANCELLATION_API(init)(void **context);

void INTERFLOP_CHECKCANCELLATION_API(add_double)(double a, double b,
                                                 double *res, void *context);
void INTERFLOP_CHECKCANCELLATION_API(add_float)(float a, float b, float *res,
                                                void *context);
void INTERFLOP_CHECKCANCELLATION_API(sub_double)(double a, double b,
                                                 double *res, void *context);
void INTERFLOP_CHECKCANCELLATION_API(sub_float)(float a, float b, float *res,
                                                void *context);
void INTERFLOP_CHECKCANCELLATION_API(mul_double)(double a, double b,
                                                 double *res, void *context);
void INTERFLOP_CHECKCANCELLATION_API(mul_float)(float a, float b, float *res,
                                                void *context);
void INTERFLOP_CHECKCANCELLATION_API(div_double)(double a, double b,
                                                 double *res, void *context);
void INTERFLOP_CHECKCANCELLATION_API(div_float)(float a, float b, float *res,
                                                void *context);

void INTERFLOP_CHECKCANCELLATION_API(cast_double_to_float)(double a, float *b,
                                                           void *context);

void INTERFLOP_CHECKCANCELLATION_API(fma_double)(double a, double b, double c,
                                                 double *res, void *context);
void INTERFLOP_CHECKCANCELLATION_API(fma_float)(float a, float b, float c,
                                                float *res, void *context);

#ifdef __cplusplus
}
#endif

#endif /* __INTERFLOP_CHECKCANCELLATION_H__ */
