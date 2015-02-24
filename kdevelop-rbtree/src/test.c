#include <stdio.h>
#include <stdlib.h>

#include "rb_tree.h"


typedef struct{
    rb_node base;
    int k,v;
} rb_node_int_int;

void rbt_insert_int_int(rb_node_int_int **proot, int k, int v){
    rb_node_int_int *parent = NULL;
    rb_node_int_int *cur_root = *proot;
    rb_node_int_int *n = (rb_node_int_int*)malloc(sizeof(rb_node_int_int));
    n->k = k;
    n->v = v;
    
    while(cur_root != NULL){
        if(n->k == cur_root->k){
            cur_root->v = n->v;
            return;
        }
        parent = cur_root;
        cur_root = (rb_node_int_int*)((n->k < cur_root->k)? cur_root->base.left: cur_root->base.right);
    }
    n->base.parent = (rb_node*)parent;
    if(parent != NULL){
        *((n->k < parent->k)? &parent->base.left: &parent->base.right) = (rb_node*)n;
    }
    rbt_after_insert((rb_node**)proot, (rb_node*)n);
}
rb_node_int_int *rbt_find_int_int(rb_node_int_int *root, int k){
    while(root != NULL){
        if(k == root->k){
            return root;
        }
        root = (rb_node_int_int*)((k < root->k)? root->base.left: root->base.right);
    }
    return NULL;
}

void rbt_print_int_int(rb_node_int_int *root){
    rb_node_int_int *iter;
    printf("{");
    for(iter=(rb_node_int_int*)rbt_min((rb_node*)root); iter!=NULL; iter=(rb_node_int_int*)rbt_next((rb_node*)iter)){
        printf("%d:%d, ", iter->k, iter->v);
    }
    printf("}\n");
}

int rb_tree_test(){
    rb_node_int_int *root = NULL;
    rb_node_int_int *p;
    rbt_insert_int_int(&root, 1,1);
    rbt_insert_int_int(&root, 2,2);
    rbt_insert_int_int(&root, 3,3);
    rbt_insert_int_int(&root, 4,4);
    rbt_print_int_int(root);
    printf("find 1: %d\n", rbt_find_int_int(root, 1)->v);
    printf("find 2: %d\n", rbt_find_int_int(root, 2)->v);
    
    p = rbt_find_int_int(root, 1);
    rbt_pop((rb_node**)&root, (rb_node*)p);
    rbt_print_int_int(root);
    
    p = rbt_find_int_int(root, 4);
    rbt_pop((rb_node**)&root, (rb_node*)p);
    rbt_print_int_int(root);
    
    p = rbt_find_int_int(root, 3);
    rbt_pop((rb_node**)&root, (rb_node*)p);
    rbt_print_int_int(root);
    
    p = rbt_find_int_int(root, 2);
    rbt_pop((rb_node**)&root, (rb_node*)p);
    rbt_print_int_int(root);
    
    return 0;
}

void test(){
    rb_tree_test();
}


int main(int argc, char **argv){
    (void)argc; (void)argv;
    
    test();
    
    return 0;
}