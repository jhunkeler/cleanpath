#ifndef CLEAN_PATH_FRAMEWORK_H
#define CLEAN_PATH_FRAMEWORK_H

#include <stdio.h>

#define TEST_SEP ":"
#define TEST_PATH "/opt/local/bin:/opt/local/sbin:/usr/local/bin:" \
                  "/usr/bin:/bin:/usr/sbin:/sbin:/opt/X11/bin:/Library/Apple/usr/bin"

#define myassert(condition, ...) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "%s:%d:%s :: ", __FILE__, __LINE__, __FUNCTION__); \
            fprintf(stderr, __VA_ARGS__); \
            return 1; \
        } \
    } while (0)

#endif //CLEAN_PATH_FRAMEWORK_H
