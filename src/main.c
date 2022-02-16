#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "cleanpath/cleanpath.h"

#if !OS_WINDOWS
#include <unistd.h>
#endif

// Make argument parsing less wordy
#define ARGM(X) strcmp(argv[i], X) == 0

// Global program path
static char *program;

// Global runtime environment
static char **environ;

int is_valid_arg(char **args, char *s) {
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

char *getenv_ex(char *s) {
    char *key, *key_end;
    char *env_var;
    key = strdup(s);
    if (!key) {
        return NULL;
    }

    key_end = strchr(key, '=');
    if (key_end) {
        *key_end = '\0';
    }
    env_var = getenv(key);
    free(key);

    if (!env_var || !strlen(env_var)) {
        return NULL;
    }
    return env_var;
}

void show_listing(struct CleanPath *path) {
    if (path == NULL) {
        return;
    }

    for (size_t i = 0; i < CLEANPATH_PART_MAX && path->part[i] != NULL; i++) {
        if (*path->part[i] == '\0') {
            continue;
        }
        puts(path->part[i]);
    }
}

static int show_default_path() {
    char buf[CLEANPATH_PART_MAX];
#if OS_WINDOWS
    // I could not find anything in MSDN about dumping default path information.
    // The internet agrees "this" is "it" (XP/Vista/8/10):
    strncpy(buf, "%SystemRoot%\\system32;%SystemRoot%;%SystemRoot%\\System32\\Wbem", sizeof(buf));
#elif HAVE_CONFSTR  // POSIX
    size_t cf_status;
    cf_status = confstr(_CS_PATH, buf, sizeof(buf));
    if (cf_status < 1) {  // 0 and -1 are possible error codes for linux and darwin
        return 1;
    }
#else
    *buf = '\0';
#endif
    puts(buf);
    return 0;
}

char *get_path(struct CleanPath *path) {
    char *result;
    result = cleanpath_read(path);
    return result;
}

void show_path(struct CleanPath *path) {
    char *result;

    result = cleanpath_read(path);
    if (result == NULL) {
        return;
    }

    printf("%s\n", result);
    free(result);
}

static void show_version() {
    puts(CLEANPATH_VERSION);
}

static void show_usage() {
    char *bname;
    bname = strrchr(program, '/');

    printf("usage: %s [-hVDAelrsEv] [pattern ...]\n", bname ? bname + 1 : program);
    printf("  --help       -h    Displays this help message\n");
    printf("  --version    -V    Displays the program's version\n");
    printf("  --default    -D    Displays default operating system PATH " CLEANPATH_MSG_NO_DEFAULT_PATH "\n");
    printf("  --list             Format output as a list\n");
    printf("  --all        -A    Apply to all environment variables\n");
    printf("  --all-list         Format --all output as a list");
    printf("  --exact      -e    Filter when pattern is an exact match (default)\n");
    printf("  --loose      -l    Filter when any part of the pattern matches\n");
    printf("  --regex      -r    Filter matches with (Extended) Regular Expressions " CLEANPATH_MSG_NO_REGEX "\n");
    printf("  --sep [str]  -s    Use custom path separator (default: ':')\n");
    printf("  --env [str]  -E    Use custom environment variable (default: PATH)\n");
}

int main(int argc, char *argv[], char *arge[]) {
    environ = arge;
    struct CleanPath *path;
    char *sep;
    char *sys_var;
    int do_all_sys_vars;
    int do_listing;
    int do_listing_all_sys_vars;
    int do_default_path;
    int filter_mode;
    size_t pattern_nelem;
    char *pattern[CLEANPATH_PART_MAX];

    program = argv[0];
    sys_var = NULL;
    sep = CLEANPATH_SEP;
    do_all_sys_vars = 0;
    do_listing = 0;
    do_listing_all_sys_vars = 0;
    do_default_path = 0;
    filter_mode = CLEANPATH_FILTER_NONE;
    pattern_nelem = 0;
    memset(pattern, 0, (sizeof(pattern) / sizeof(char *)) * sizeof(char *));

    char *args_valid[] = {
            "--help", "-h",
            "--version", "-V",
            "--list",
            "--default", "-D",
            "--all", "-A",
            "--all-list",
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
        if (ARGM("--list")) {
            do_listing = 1;
        }
        if (ARGM("--all") || ARGM("-A")) {
            do_all_sys_vars = 1;
        }
        if (ARGM("--all-list")) {
            do_all_sys_vars = 1;
            do_listing_all_sys_vars = 1;
        }
        if (ARGM("--default") || ARGM("-D")) {
            do_default_path = 1;
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
#if OS_WINDOWS
            fprintf(stderr, "WARNING: --regex|-r is not implemented on Windows. Using default filter mode.\n");
#endif
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

    // Show default system path, and exit
    if (do_default_path) {
         exit(show_default_path());
    }

    // Use default environment variable when not set by --var
    if (sys_var == NULL) {
        sys_var = getenv_ex(CLEANPATH_VAR);
    }

    char *deferred[1024] = {0};
    size_t dcount = 0;

    // Initialize path data
    if (do_all_sys_vars) {
        for (size_t i = 0; environ[i] != NULL; i++) {
            char *var = getenv_ex(environ[i]);
            path = cleanpath_init(var, sep);
            if (path == NULL) {
                exit(1);
            }

            char *key = strdup(environ[i]);
            char *key_end = strchr(key, '=');
            if (key_end) {
                *key_end = '\0';
            }

            // Don't touch exported functions
            if (!strncmp(key, "BASH_FUNC_", strlen("BASH_FUNC_")) || !strncmp(path->data, "()", strlen("()"))) {
                free(path);
                free(key);
                continue;
            }

            // Remove patterns from sys_var
            for (size_t p = 0; p < CLEANPATH_PART_MAX && pattern[p] != NULL; p++) {
                cleanpath_filter(path, filter_mode, pattern[p]);
            }

            // Print filtered result
            if (do_listing) {
                show_listing(path);
            } else {
                char *data = get_path(path);
                if (strlen(path->data) == 0 && dcount < sizeof(deferred) / sizeof(*deferred)) {
                    deferred[dcount] = strdup(key);
                    dcount++;
                } else {
                    printf("%s='%s'; ", key, data);
                    if (do_listing_all_sys_vars) {
                        puts("");
                    }
                }
                free(data);
            }
            free(key);
            free(path);
        }

        for (size_t i = 0; i < dcount; i++) {
            printf("unset %s; ", deferred[i]);
            if (do_listing_all_sys_vars) {
                puts("");
            }
            free(deferred[i]);
        }
    } else {
        path = cleanpath_init(sys_var, sep);
        if (path == NULL) {
            exit(1);
        }

        // Remove patterns from sys_var
        for (size_t i = 0; i < CLEANPATH_PART_MAX && pattern[i] != NULL; i++) {
            cleanpath_filter(path, filter_mode, pattern[i]);
        }

        // Print filtered result
        if (do_listing) {
            show_listing(path);
        } else {
            show_path(path);
        }

        // Clean up
        cleanpath_free(path);
    }

    return 0;
}
