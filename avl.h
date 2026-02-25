// Tadas Bytautas | tadas.bytautas@mif.stud.vu.lt | 2026-02-22
// AVL medis: paieška/įterpimas/šalinimas blogiausiu atveju O(log n).

#ifndef AVL_H
#define AVL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct AvlTree AvlTree;

// Sukuria medį.
// Grąžina: 1 - jei pavyko, 0 - nepavyko
int avl_create(AvlTree** out_tree);

// Atlaisvina medį.
void avl_destroy(AvlTree* tree);

// Įterpia vertę.
// Grąžina: 1 - įterpta, 0 - dubliuojasi, -1 - nepavyko.
int avl_insert(AvlTree* tree, int key);

// Pašalina vertę.
// Grąžina: 1 - pašalinta, 0 - neegzistuoja, -1 - nepavyko.
int avl_remove(AvlTree* tree, int key);

// Patikrina ar vertė yra. 1 - yra, 0 - nėra.
int avl_contains(const AvlTree* tree, int key);

// Aplankymo funkcija inorder ėjimui.
typedef void (*AvlVisitFn)(int key, void* user_data);

// Inorder ėjimas.
void avl_inorder(const AvlTree* tree, AvlVisitFn visit, void* user_data);

#ifdef __cplusplus
}
#endif

#endif

