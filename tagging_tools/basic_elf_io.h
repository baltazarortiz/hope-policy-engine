/*
 * Copyright © 2017-2018 Dover Microsystems, Inc.
 * All rights reserved. 
 *
 * Use and disclosure subject to the following license. 
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef BASIC_ELF_IO_H
#define BASIC_ELF_IO_H

#include <stdio.h>
#include <stdarg.h>

#include "elf_loader.h"

namespace policy_engine {

/*
  Herein are some utility classes that implement the basic APIs that are needed by our ELF loader
  to load ELF files for utilities.  These implementations go to FILE * objects for I/O.
*/

/**
   Basic reporter class that just talks to STDIO.
*/
struct stdio_reporter_t : reporter_t {
  int errors;
  int warnings;

  stdio_reporter_t() : errors(0), warnings(0) { }

  void error(const char *fmt, ...) {
    fprintf(stderr, "error: ");
    va_list vl;
    va_start(vl, fmt);
    vfprintf(stderr, fmt, vl);
    va_end(vl);
    errors++;
  }
  void warning(const char *fmt, ...) {
    fprintf(stderr, "warning: ");
    va_list vl;
    va_start(vl, fmt);
    vfprintf(stderr, fmt, vl);
    va_end(vl);
    warnings++;
  }
  void info(const char *fmt, ...) {
    printf("info: ");
    va_list vl;
    va_start(vl, fmt);
    vprintf(fmt, vl);
    va_end(vl);
  }
};

/**
   File reader that reads its bits from a FILE *.
*/
struct FILE_reader_t : file_stream_t {

  FILE_reader_t(FILE *fp) : fp(fp) { }
  bool read(void *buff, size_t size) {
    if (fread(buff, 1, size, fp) != size)
      return false;
    return true;
  }

  bool seek(size_t where, whence_t whence) {
    int w;
    switch (whence) {
    case CUR: w = SEEK_CUR; break;
    case SET: w = SEEK_SET; break;
    default: return false;
    }
    if (fseek(fp, where, w))
      return false;
    return true;
  }
  
  FILE *fp;
};

} // namespace policy_engine

#endif
