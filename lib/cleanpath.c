#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "cleanpath.h"

#if !OS_WINDOWS
#include <regex.h>
#endif

/**
 * Split path into parts by sep
 * @param path a string (e.g. "/path1:/path2:/pathN")
 * @param sep a delimiter string (e.g. ":")
 * @note CleanPath->data string is modified. part are pointers to addresses of path (i.e. do not free())
 * @return a pointer to a CleanPath structure
 */
struct CleanPath *cleanpath_init(const char *path, const char *sep) {
    struct CleanPath *result;
    char *elem;
    size_t i;
    result = NULL;

    if (!path || !sep) {
        goto cleanpath_init__failure;
    }

    result = calloc(1, sizeof(*result));
    if (result == NULL) {
        goto cleanpath_init__failure;
    }

    result->data = strdup(path);
    if (result->data == NULL) {
        return NULL;
    }

    result->data_len = strlen(result->data) + 2; // + 2 to handle an empty PATH
    result->sep = strdup(sep);
    if (result->sep == NULL) {
        goto cleanpath_init__failure;
    }

    // Split string by separator
    elem = strtok(result->data, sep);
    for (i = 0; elem != NULL; i++) {
        result->part[i] = elem;
        elem = strtok(NULL, sep);
    }
    result->part_nelem = i;

    cleanpath_init__failure:
    return result;
}

/**
 * Filter elements in CleanPath->part array by pattern
 * @param path a pointer to a CleanPath structure
 * @param mode an integer CLEANPATH_FILTER_EXACT, CLEANPATH_FILTER_LOOSE, or CLEANPATH_FILTER_REGEX
 * @param pattern a string
 * @see cleanpath_init()
 * @note CLEANPATH_FILTER_EXACT is the default mode
 */
void cleanpath_filter(struct CleanPath *path, unsigned mode, const char *pattern) {
    int match;
    regex_t regex;

    for (size_t i = 0; path->part[i]; i++) {
        match = 0;
        switch(mode) {
            case CLEANPATH_FILTER_LOOSE:
                match = strstr(path->part[i], pattern) != NULL ? 1 : 0;
                break;
            case CLEANPATH_FILTER_REGEX:
                if (regcomp(&regex, pattern, REG_EXTENDED | REG_NOSUB) != 0) {
                    return;
                }
                match = regexec(&regex, path->part[i], 0, NULL, 0) == 0 ? 1 : 0;
                regfree(&regex);
                break;
            case CLEANPATH_FILTER_EXACT:
            default:
                match = strcmp(path->part[i], pattern) == 0 ? 1 : 0;
                break;
        }

        if (match)
            *path->part[i] = '\0';
    }
}

/**
 * Reconstruct CleanPath->data using CleanPath->part and return a copy of the string
 * @param path a pointer to a CleanPath structure
 * @see cleanpath_filter()
 * @return a string
 */
char *cleanpath_read(struct CleanPath *path) {
    char *result;

    result = calloc(path->data_len, sizeof(char));
    if (result == NULL) {
        goto cleanpath_read__failure;
    }

    for (size_t i = 0; path->part[i] != NULL; i++) {
        // Ignore filtered paths
        if (*path->part[i] == '\0') {
            continue;
        }
        strcat(result, path->part[i]);

        // Do not append path separator on final element
        if (path->part[i + 1] != NULL && *path->part[i + 1] != '\0') {
            strcat(result, path->sep);
        }
    }

    cleanpath_read__failure:
    return result;
}

/**
 * Free memory allocated by cleanpath_init()
 * @param path a pointer to a CleanPath structure
 */
void cleanpath_free(struct CleanPath *path) {
    free(path->data);
    free(path->sep);
    free(path);
}

