#include <stdio.h>
#include "stack.c"

//中序遍历
int inorder_tree_walk(node *x)
{
    if(x != NULL){
        inorder_tree_walk(x->left);
        printf("%d\n", x->key);
        inorder_tree_walk(x->right);
    }
}
//通过栈实现中序遍历
int inorder_tree_walk_by_stack(node *x)
{
    stack *s;
    while(x != NULL && !stack_empty(s)){
        if(x != NULL){
            push(s, x);
        }
        if(x->left != NULL){
            x = x->left;
        }else{
            x = s->top;
            pop(s);
            printf("%d\n", x->key);
            if(x->right != NULL){
                x = x->right;
            }
        }
    }
}
//更好的中序遍历
int inorder_tree_walk_best(node *x, int *A)
{
    int flag = 0;
    while(x != NULL){
        if(x->left != NULL && flag == 0){
            x = x->left;
        }else{
            flag = 1;//标识已经到最左孩子
            if(array[x->key] == 0){
                printf("%d\n", x->key);
                array[x->key] = 1;
            }
            if(x->right != NULL){
                x = x->right;
                flag = 0;//标识从当前节点开始查找左孩子
            }else{
                x = x->p;
            }
        }
    }
}
//前序遍历
int preorder_tree_walk(node *x)
{
    if(x != NULL){
        printf("%d\n", x->key);
        preprder_tree_walk(x->left);
        preprder_tree_walk(x->right);
    }
}
//通过栈实现的先序遍历
int preorder_tree_walk_by_stack(node *x)
{
    stack *s;
    while(x != NULL && !stack_empty(s)){
        while(x != NULL){
            printf("%d\n", x->key);
            push(s,x);
            x = x->left;
        }
        x = s->top;
        pop(s);
        x = x->right;
    }
}
//后序遍历
int postorder_tree_walk(node *x)
{
    if(x != NULL){
        postorder_tree_walk(x->left);
        postorder_tree_walk(x->right);
        printf("%d\n", x->key);
    }
}
//通过栈实现的后序遍历
// int postorder_tree_walk_by_stack(node *x)
// {
//     
// }
