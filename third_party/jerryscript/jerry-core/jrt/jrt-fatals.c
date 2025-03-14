/* Copyright 2014-2016 Samsung Electronics Co., Ltd.
 * Copyright 2016 University of Szeged.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * Implementation of exit with specified status code.
 */

#include "jrt.h"
#include "jrt-libc-includes.h"

#define JERRY_INTERNAL
#include "jerry-internal.h"

static void __noreturn
jerry_fatal_with_lr (jerry_fatal_code_t code, /**< status code */
                     void *lr) /**< return address at time the failure occurred */
{
#ifndef JERRY_NDEBUG
  switch (code)
  {
    case ERR_OUT_OF_MEMORY:
    {
      JERRY_ERROR_MSG ("Error: ERR_OUT_OF_MEMORY\n");
      break;
    }
    case ERR_SYSCALL:
    {
      /* print nothing as it may invoke syscall recursively */
      break;
    }
    case ERR_REF_COUNT_LIMIT:
    {
      JERRY_ERROR_MSG ("Error: ERR_REF_COUNT_LIMIT\n");
      break;
    }
    case ERR_FAILED_INTERNAL_ASSERTION:
    {
      JERRY_ERROR_MSG ("Error: ERR_FAILED_INTERNAL_ASSERTION\n");
      break;
    }
  }
#endif /* !JERRY_NDEBUG */

  jerry_port_fatal (code, lr);

  /* to make compiler happy for some RTOS: 'control reaches end of non-void function' */
  while (true)
  {
  }
} /* jerry_fatal_with_lr */


/*
 * Exit with specified status code.
 *
 * If !JERRY_NDEBUG and code != 0, print status code with description
 * and call assertion fail handler.
 */
void __noreturn
jerry_fatal (jerry_fatal_code_t code) /**< status code */
{
  jerry_fatal_with_lr(code, JERRY_RETURN_ADDRESS);
} /* jerry_fatal */

#ifndef JERRY_NDEBUG
/**
 * Handle failed assertion
 */
void __noreturn
jerry_assert_fail (const char *assertion, /**< assertion condition string */
                   const char *file, /**< file name */
                   const char *function, /**< function name */
                   const uint32_t line) /**< line */
{
  JERRY_ERROR_MSG ("ICE: Assertion '%s' failed at %s(%s):%lu.\n",
                   assertion,
                   file,
                   function,
                   (unsigned long) line);

  jerry_fatal_with_lr (ERR_FAILED_INTERNAL_ASSERTION, JERRY_RETURN_ADDRESS);
} /* jerry_assert_fail */

/**
 * Handle execution of control path that should be unreachable
 */
void __noreturn
jerry_unreachable (const char *file, /**< file name */
                   const char *function, /**< function name */
                   const uint32_t line) /**< line */
{
  JERRY_ERROR_MSG ("ICE: Unreachable control path at %s(%s):%lu was executed.\n",
                   file,
                   function,
                   (unsigned long) line);

  jerry_fatal_with_lr (ERR_FAILED_INTERNAL_ASSERTION, JERRY_RETURN_ADDRESS);
} /* jerry_unreachable */
#endif /* !JERRY_NDEBUG */
