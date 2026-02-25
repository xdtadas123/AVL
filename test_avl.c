#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "avl.h"

struct PrintCtx {
    char* buf;
    size_t cap;
    size_t len;
};

static void visit_print(int key, void* user_data)
{
    struct PrintCtx* ctx = (struct PrintCtx*)user_data;

    int wrote = snprintf(ctx->buf + ctx->len, ctx->cap - ctx->len, "%d ", key);
    assert(wrote > 0);
    assert(ctx->len + (size_t)wrote < ctx->cap);

    ctx->len += (size_t)wrote;
}

int main()
{
    AvlTree* t = NULL;
    assert(avl_create(&t) == 1);
    assert(t != NULL);
    avl_destroy(NULL);

    assert(avl_contains(t, 10) == 0);
    assert(avl_insert(t, 10) == 1);
    assert(avl_insert(t, 10) == 0);
    assert(avl_contains(t, 10) == 1);

    assert(avl_insert(t, 5) == 1);
    assert(avl_insert(t, 20) == 1);
    assert(avl_contains(t, 5) == 1);
    assert(avl_contains(t, 20) == 1);
    assert(avl_contains(t, 999) == 0);

    {
        char out[64] = {0};
        struct PrintCtx ctx = { out, sizeof(out), 0 };

        avl_inorder(t, visit_print, &ctx);

        assert(strcmp(out, "5 10 20 ") == 0);
    }

    assert(avl_remove(t, 999) == 0);
    assert(avl_remove(t, 10) == 1);
    assert(avl_contains(t, 10) == 0);

    assert(avl_insert(NULL, 1) == -1);
    assert(avl_remove(NULL, 1) == -1);
    assert(avl_contains(NULL, 1) == 0);
    assert(avl_create(NULL) == 0);

    avl_destroy(t);

    printf("OK\n");
    return 0;
}
