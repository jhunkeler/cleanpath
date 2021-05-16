#include "cleanpath/cleanpath.h"
#include "framework.h"

int main() {
    struct CleanPath *path;
    path = NULL;

    //intentionally bad
    myassert((path = cleanpath_init("good", NULL)) == NULL, "cleanpath_init() returned non-NULL on NULL sep\n");
    if (path) cleanpath_free(path);
    myassert((path = cleanpath_init(NULL, "good")) == NULL, "cleanpath_init() returned non-NULL on NULL path\n");
    if (path) cleanpath_free(path);
}
