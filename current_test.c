#include "xor_tree.h"
#include "pivlib.h"
#include <stdio.h>

int main() {
    struct xor_node tree1;
    tree1.rchild = 1;
    tree1.lchild = 0 + 20;

    struct xor_tree_iter it;
    it.node = &tree1;
    printf("lchild = %lu\n", xor_tree_lsubtree(it));
}
