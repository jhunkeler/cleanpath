#ifndef CLEANPATH_CLEANPATH_H
#define CLEANPATH_CLEANPATH_H

// Define some platform detection shortcuts
#define OS_DARWIN 0
#define OS_WINDOWS 0
#define OS_LINUX 0
#define OS_SUPPORTED 0

#if defined(__APPLE__) && defined(__MACH__)
#   undef OS_DARWIN
#   define OS_DARWIN 1
#   undef OS_SUPPORTED
#   define OS_SUPPORTED 1

#elif defined(_WIN32)
#   undef OS_WINDOWS
#   define OS_WINDOWS 1

#elif defined(__linux) || defined(__linux__)
#   undef OS_LINUX
#   define OS_LINUX 1
#   undef OS_SUPPORTED
#   define OS_SUPPORTED 1
#endif

#include <stdlib.h>
#include <string.h>

#define CLEANPATH_VERSION "0.1.0"
#define CLEANPATH_FILTER_NONE -1
#define CLEANPATH_FILTER_EXACT 0
#define CLEANPATH_FILTER_LOOSE 1
#define CLEANPATH_FILTER_REGEX 2
#define CLEANPATH_PART_MAX 1024
#define CLEANPATH_VAR "PATH"
#define CLEANPATH_SEP ":"

#if OS_WINDOWS
#undef CLEANPATH_SEP
#define CLEANPATH_SEP ";"
#endif

struct CleanPath {
    char *data;                      // Pointer to the path string
    size_t data_len;                 // Length of the path string
    char *sep;                       // Pointer to the separator used to split the data string
    char *part[CLEANPATH_PART_MAX];  // Array of pointers to path elements
    size_t part_nelem;               // Total number of elements in part array
};

// Prototypes
struct CleanPath *cleanpath_init(const char *path, const char *sep);
void cleanpath_filter(struct CleanPath *path, unsigned mode, const char *pattern);
char *cleanpath_read(struct CleanPath *path);
void cleanpath_free(struct CleanPath *path);

#endif //CLEANPATH_CLEANPATH_H