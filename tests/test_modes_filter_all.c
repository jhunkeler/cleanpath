#include <stdio.h>
#include "cleanpath.h"
#include "framework.h"

#define MAX_MODE 3
#define MAX_PART CLEANPATH_PART_MAX
#define MAX_RECORD 255

#if OS_WINDOWS
// Disable regex filter test
#undef MAX_MODE
#define MAX_MODE 2
#endif

unsigned modes[MAX_MODE] = {
    CLEANPATH_FILTER_EXACT,
    CLEANPATH_FILTER_LOOSE,
#if !OS_WINDOWS
    CLEANPATH_FILTER_REGEX,
#endif
};

char *modes_str[MAX_MODE] = {
    "exact",
    "loose",
#if !OS_WINDOWS
    "regex",
#endif
};

const char *inputs[MAX_MODE][MAX_PART][MAX_RECORD] = {
    {   // Filter exact
        "/opt/local/bin",
        "/opt/local/sbin",
        "/usr/local/bin",
        "/usr/bin",
        "/bin",
        "/usr/sbin",
        "/sbin",
        "/opt/X11/bin",
        "/Library/Apple/usr/bin",
        NULL
    },
    {   // Filter loose
        "bin",
        NULL
    },
#if !OS_WINDOWS
    {   // Filter regex
        ".*",
        NULL
    },
#endif
};

struct TestResult {
    size_t status;
    size_t mode;
    char *output;
    char input[1024];
    char sep[25];
};

int main() {
    int status;
    struct TestResult tr[MAX_MODE];

    status = 0;
    for (size_t mode = 0; mode < MAX_MODE; mode++) {
        struct CleanPath *path;
        strcpy(tr[mode].sep, TEST_SEP);
        strcpy(tr[mode].input, TEST_PATH);
        tr[mode].mode = mode;
        tr[mode].status = 0;
        tr[mode].output = NULL;

        path = cleanpath_init(tr[mode].input, tr[mode].sep);
        for (size_t part = 0; part < MAX_PART && inputs[mode][part] != NULL; part++) {
            for (size_t record = 0; inputs[mode][part][record] != NULL; record++) {
                cleanpath_filter(path, mode, inputs[mode][part][record]);
            }
        }
        tr[mode].output = cleanpath_read(path);
        tr[mode].status = strlen(tr[mode].output) == 0;
        cleanpath_free(path);
    }

    for (size_t i = 0; i < MAX_MODE; i++) {
        if (tr[i].status != 0) {
            free(tr[i].output);
            continue;
        }
        status++;
        fprintf(stderr, "'%s' mode failed to filter elements from:\n'%s'\nRemainder:\n'%s'\n",
                modes_str[tr[i].mode], tr[i].input, tr[i].output);
        free(tr[i].output);
    }

    myassert(status == 0, "%d mode(s) failed", status);
    return 0;
}