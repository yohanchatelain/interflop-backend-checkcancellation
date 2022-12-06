
/*--------------------------------------------------------------------*/
/*--- Verrou: a FPU instrumentation tool.                          ---*/
/*--- Interface for floating-point operations overloading.         ---*/
/*---                                                 vr_fpOps.cxx ---*/
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

#include <argp.h>
#include <stddef.h>

#include "interflop-stdlib/interflop.h"
#include "interflop-stdlib/interflop_stdlib.h"
#include "interflop-stdlib/iostream/logger.h"
#include "interflop_checkcancellation.h"
#include "vr_fpRepr.hxx"

static File *stderr_stream;

checkcancellation_conf_t checkcancellation_conf;

static void
_set_checkcancellation_threshold_binary32(long val,
                                          checkcancellation_context_t *ctx) {
  ctx->threshold_b32 = val;
}

static void
_set_checkcancellation_threshold_binary64(long val,
                                          checkcancellation_context_t *ctx) {
  ctx->threshold_b64 = val;
}

template <typename REAL>
void ifcc_checkCancellation(const REAL &a, const REAL &b, const REAL &r);

// * Global variables & parameters

template <typename REAL> int ifcc_threshold(const REAL &a);

template <> int ifcc_threshold(const float &a) {
  return checkcancellation_conf.threshold_b32;
}
template <> int ifcc_threshold(const double &a) {
  return checkcancellation_conf.threshold_b64;
}

template <typename REAL>
inline void ifcc_checkCancellation(const REAL &a, const REAL &b,
                                   const REAL &r) {

  const int ea = exponentField(a);
  const int eb = exponentField(b);
  const int er = exponentField(r);

  const int emax = ea > eb ? ea : eb;
  const int cancelled = emax - er;

  if (cancelled >= ifcc_threshold(a)) {
    interflop_cancellationHandler(cancelled);
  }
}

// * C interface
void INTERFLOP_CHECKCANCELLATION_API(configure)(checkcancellation_conf_t conf,
                                                void *context) {
  checkcancellation_context_t *ctx = (checkcancellation_context_t *)context;
  ctx->threshold_b64 = conf.threshold_b64;
  ctx->threshold_b32 = conf.threshold_b32;
}

void INTERFLOP_CHECKCANCELLATION_API(finalize)(void *context) {}

const char *INTERFLOP_CHECKCANCELLATION_API(get_backend_name)() {
  return "checkcancellation";
}

const char *INTERFLOP_CHECKCANCELLATION_API(get_backend_version)() {
  return "1.x-dev";
}

void INTERFLOP_CHECKCANCELLATION_API(add_double)(double a, double b,
                                                 double *res, void *context) {
  ifcc_checkCancellation(a, b, *res);
}

void INTERFLOP_CHECKCANCELLATION_API(add_float)(float a, float b, float *res,
                                                void *context) {
  ifcc_checkCancellation(a, b, *res);
}

void INTERFLOP_CHECKCANCELLATION_API(sub_double)(double a, double b,
                                                 double *res, void *context) {
  ifcc_checkCancellation(a, b, *res);
}

void INTERFLOP_CHECKCANCELLATION_API(sub_float)(float a, float b, float *res,
                                                void *context) {
  ifcc_checkCancellation(a, b, *res);
}

void INTERFLOP_CHECKCANCELLATION_API(mul_double)(double a, double b,
                                                 double *res, void *context) {
  *res = a * b;
}

void INTERFLOP_CHECKCANCELLATION_API(mul_float)(float a, float b, float *res,
                                                void *context) {
  *res = a * b;
}

void INTERFLOP_CHECKCANCELLATION_API(div_double)(double a, double b,
                                                 double *res, void *context) {
  *res = a / b;
}

void INTERFLOP_CHECKCANCELLATION_API(div_float)(float a, float b, float *res,
                                                void *context) {
  *res = a / b;
}

void INTERFLOP_CHECKCANCELLATION_API(fma_double)(double a, double b, double c,
                                                 double *res, void *context) {
  ifcc_checkCancellation(a * b, c, *res);
}

void INTERFLOP_CHECKCANCELLATION_API(fma_float)(float a, float b, float c,
                                                float *res, void *context) {
  ifcc_checkCancellation(a * b, c, *res);
}

#define CHECK_IMPL(name)                                                       \
  if (interflop_##name == Null) {                                              \
    interflop_panic("Interflop backend error: " #name " not implemented\n");   \
  }

void _checkcancellation_check_stdlib(void) {
  CHECK_IMPL(exit);
  CHECK_IMPL(fprintf);
  CHECK_IMPL(cancellationHandler);
  CHECK_IMPL(malloc);
  CHECK_IMPL(strcasecmp);
  CHECK_IMPL(strtol);
}

void _checkcancellation_alloc_context(void **context) {
  *context = (checkcancellation_context_t *)interflop_malloc(
      sizeof(checkcancellation_context_t));
}

void _checkcancellation_init_context(checkcancellation_context_t *ctx) {
  ctx->threshold_b64 = 0;
  ctx->threshold_b32 = 0;
}

void INTERFLOP_CHECKCANCELLATION_API(pre_init)(File *stream,
                                               interflop_panic_t panic,
                                               void **context) {
  stderr_stream = stream;
  interflop_set_handler("panic", (void *)panic);
  _checkcancellation_check_stdlib();
  _checkcancellation_alloc_context(context);
  checkcancellation_context_t *ctx = (checkcancellation_context_t *)*context;
  _checkcancellation_init_context(ctx);
}

typedef enum { KEY_THRESHOLD_BINARY32, KEY_THRESHOLD_BINARY64 } key_args;

static const char key_threshold_b32_str[] = "threshold-binary32";
static const char key_threshold_b64_str[] = "threshold-binary64";

static struct argp_option options[] = {
    {key_threshold_b32_str, KEY_THRESHOLD_BINARY32, "THRESHOLD", 0,
     "select cancellation threshold for binary32", 0},
    {key_threshold_b64_str, KEY_THRESHOLD_BINARY64, "THRESHOLD", 0,
     "select cancellation threshold for binary64", 0},
    {0}};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  checkcancellation_context_t *ctx =
      (checkcancellation_context_t *)state->input;
  int error = 0;
  long val = -1;
  char *endptr;
  switch (key) {
  case KEY_THRESHOLD_BINARY32:
    /* cancellation threshold for binary32 */
    error = 0;
    val = interflop_strtol(arg, &endptr, &error);
    if (error != 0 || val <= 0) {
      logger_error("--%s invalid value provided, must be a positive integer",
                   key_threshold_b32_str);
    } else {
      _set_checkcancellation_threshold_binary32(val, ctx);
    }
    break;
  case KEY_THRESHOLD_BINARY64:
    /* cancellation threshold for binary64 */
    error = 0;
    val = interflop_strtol(arg, &endptr, &error);
    if (error != 0 || val <= 0) {
      logger_error("--%s invalid value provided, must be a positive integer",
                   key_threshold_b64_str);
    } else {
      _set_checkcancellation_threshold_binary64(val, ctx);
    }
    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp argp = {options, parse_opt, "", "", NULL, NULL, NULL};

void INTERFLOP_CHECKCANCELLATION_API(CLI)(int argc, char **argv,
                                          void *context) {
  checkcancellation_context_t *ctx = (checkcancellation_context_t *)context;
  if (interflop_argp_parse != NULL) {
    interflop_argp_parse(&argp, argc, argv, 0, 0, ctx);
  } else {
    interflop_panic("Interflop backend error: argp_parse not implemented\n"
                    "Provide implementation or use interflop_configure to "
                    "configure the backend\n");
  }
}

struct interflop_backend_interface_t
INTERFLOP_CHECKCANCELLATION_API(init)(void *context) {
  struct interflop_backend_interface_t interflop_backend_checkcancellation = {
    interflop_add_float : INTERFLOP_CHECKCANCELLATION_API(add_float),
    interflop_sub_float : INTERFLOP_CHECKCANCELLATION_API(sub_float),
    interflop_mul_float : INTERFLOP_CHECKCANCELLATION_API(mul_float),
    interflop_div_float : INTERFLOP_CHECKCANCELLATION_API(div_float),
    interflop_cmp_float : Null,
    interflop_add_double : INTERFLOP_CHECKCANCELLATION_API(add_double),
    interflop_sub_double : INTERFLOP_CHECKCANCELLATION_API(sub_double),
    interflop_mul_double : INTERFLOP_CHECKCANCELLATION_API(mul_double),
    interflop_div_double : INTERFLOP_CHECKCANCELLATION_API(div_double),
    interflop_cmp_double : Null,
    interflop_cast_double_to_float : Null,
    interflop_fma_float : INTERFLOP_CHECKCANCELLATION_API(fma_float),
    interflop_fma_double : INTERFLOP_CHECKCANCELLATION_API(fma_double),
    interflop_enter_function : Null,
    interflop_exit_function : Null,
    interflop_user_call : Null,
    interflop_finalize : Null,
  };

  return interflop_backend_checkcancellation;
}

struct interflop_backend_interface_t interflop_init(void *context)
    __attribute__((weak, alias("interflop_checkcancellation_init")));

void interflop_pre_init(File *stream, interflop_panic_t panic, void **context)
    __attribute__((weak, alias("interflop_checkcancellation_pre_init")));

void interflop_CLI(int argc, char **argv, void *context)
    __attribute__((weak, alias("interflop_checkcancellation_CLI")));
