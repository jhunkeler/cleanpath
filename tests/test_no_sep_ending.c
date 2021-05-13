#include "cleanpath/cleanpath.h"
#include "framework.h"

int main() {
    const char *input = "/usr/bin:/usr/sbin:/sbin:/bin:";
    struct CleanPath *path;
    char *result;
    size_t len;

    result = NULL;
    path = cleanpath_init(input, TEST_SEP);
    cleanpath_filter(path, CLEANPATH_FILTER_REGEX, "/bin");
    result = cleanpath_read(path);
    cleanpath_free(path);

    len = strlen(result);
    puts(result);
    myassert(len && *(result + (len - 1)) != TEST_SEP[0], "Result ends with a separator:\n'%s'", result);
}
