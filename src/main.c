#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "cleanpath.h"

// Make argument parsing less wordy
#define ARGM(X) strcmp(argv[i], X) == 0

// Global program path
static char *program;

static int is_valid_arg(char **args, char *s) {
    int match;

    match = 0;

    // cause positional arguments to fall through as valid
    if (*s != '-') {
        return 1;
    }

    for (size_t i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], s) == 0) {
            match = 1;
        }
    }

    return match;
}


static char *getenv_ex(char *s) {
    char *env_var;
    env_var = getenv(s);
    if (!env_var || !strlen(env_var)) {
        return NULL;
    }
    return env_var;
}

static void show_version() {
    puts(CLEANPATH_VERSION);
}

static void show_usage() {
    char *bname;
    bname = strrchr(program, '/');

    printf("usage: %s [-hVelrsv] [pattern ...]\n", bname ? bname + 1 : program);
    printf("  --help       -h    Displays this help message\n");
    printf("  --version    -V    Displays the program's version\n");
    printf("  --exact      -e    Filter when pattern is an exact match (default)\n");
    printf("  --loose      -l    Filter when any part of the pattern matches\n");
    printf("  --regex      -r    Filter matches with (Extended) Regular Expressions\n");
    printf("  --sep [str]  -s    Use custom path separator (default: ':')\n");
    printf("  --env [str]  -E    Use custom environment variable (default: PATH)\n");
}

int main(int argc, char *argv[]) {
    struct CleanPath *path;
    char *sep;
    char *sys_var;
    char *result;
    size_t pattern_nelem;
    int filter_mode;
    int args_invalid;
    char *pattern[CLEANPATH_PART_MAX];

    program = argv[0];
    result = NULL;
    sys_var = NULL;
    sep = CLEANPATH_SEP;
    filter_mode = CLEANPATH_FILTER_NONE;
    memset(pattern, 0, sizeof(pattern) / sizeof(char *));
    pattern_nelem = 0;

    args_invalid = 0;
    char *args_valid[] = {
            "--help", "-h",
            "--version", "-V",
            "--exact", "-e",
            "--loose", "-l",
            "--regex", "-r",
            "--sep", "-s",
            "--env", "-E",
            NULL
    };

    for (int i = 1; i < argc; i++) {
        if (!is_valid_arg(args_valid, argv[i])) {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            args_invalid = 1;
            continue;
        }
        if (args_invalid) {
            exit(1);
        }
        if (ARGM("--help") || ARGM("-h")) {
            show_usage();
            exit(0);
        }
        if (ARGM("--version") || ARGM("-V")) {
            show_version();
            exit(0);
        }
        if (ARGM("--exact") || ARGM("-e")) {
            filter_mode = CLEANPATH_FILTER_EXACT;
            continue;
        }
        if (ARGM("--loose") || ARGM("-l")) {
            filter_mode = CLEANPATH_FILTER_LOOSE;
            continue;
        }
        if (ARGM("--regex") || ARGM("-r")) {
            filter_mode = CLEANPATH_FILTER_REGEX;
            continue;
        }
        if (ARGM("--sep") || ARGM("-s")) {
            i++;
            sep = argv[i];
            continue;
        }
        if (ARGM("--env") || ARGM("-E")) {
            i++;
            sys_var = getenv_ex(argv[i]);
            if (!sys_var) {
                exit(1);
            }
            continue;
        }

        if (filter_mode == CLEANPATH_FILTER_NONE) {
            filter_mode = CLEANPATH_FILTER_EXACT;
        }

        // Record filter patterns
        pattern[pattern_nelem] = argv[i];
        pattern_nelem++;
    }

    // Use default environment variable when not set by --var
    if (sys_var == NULL) {
        sys_var = getenv_ex(CLEANPATH_VAR);
    }

    // Initialize path data
    path = cleanpath_init(sys_var, sep);
    if (path == NULL) {
        exit(1);
    }

    // Remove patterns from sys_var
    for (size_t i = 0; pattern[i] != NULL; i++) {
        cleanpath_filter(path, filter_mode, pattern[i]);
    }

    // Print filtered result
    result = cleanpath_read(path);
    printf("%s\n", result);

    // Clean up
    free(result);
    cleanpath_free(path);

    return 0;
}
