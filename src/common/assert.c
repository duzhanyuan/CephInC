#include "common/assert.h"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <execinfo.h>

#include "common/log.h"

void __cceph_assert_fail(const char *assertion, 
                         const char *file, int line, const char *func) {
    //log the assertion and its position
    char buf[8096];
    snprintf(buf, sizeof(buf),
            "%s: In function '%s' thread %llx \n"
            "%s: %d: FAILED assert(%s)\n",
            file, func, (unsigned long long)pthread_self(), 
            file, line, assertion);
    LOG(LL_FATAL, buf);

    //log the backtrace
    int   max = 100;
    void* array[max];
    int   size = backtrace(array, max);
    char  **strings = backtrace_symbols(array, size);
    int   i = 0;
    for (i = 0; i < size; i++) {
        LOG(LL_INFO, "    %p:%s\n", array[i], strings[i]);
    }

    //fail
    exit(1);
  }

void __ceph_assert_warn(const char *assertion, const char *file,
                        int line, const char *func) {
    char buf[8096];
    snprintf(buf, sizeof(buf),
            "WARNING: assert(%s) at: %s: %d: %s()\n",
            assertion, file, line, func);
    LOG(LL_WARN, buf);
}
