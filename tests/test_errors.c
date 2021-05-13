#include "cleanpath/cleanpath.h"
#include "framework.h"

int main() {
    struct CleanPath *path;
    path = NULL;

    //intentionally bad
    myassert(cleanpath_init("good", NULL) == NULL, "cleanpath_init() returned non-NULL on NULL sep\n");
    myassert(cleanpath_init(NULL, "good") == NULL, "cleanpath_init() returned non-NULL on NULL path\n");
}
