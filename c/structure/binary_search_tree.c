#include <stdio.h>
#include <malloc.h>
#include <string.h>

#define nil -1

//节点定义
typedef struct _node
{
    struct _node *p;
    struct _node *left;
    struct _node *right;
    int key;
} node;
//树定义
typedef struct _tree
{
    node *root;
} tree;
//新建节点
node * new_node()
{
    node *new;
    new = (node *)malloc(sizeof(node));
    new->p     = NULL;
    new->left  = NULL;
    new->right = NULL;
    new->key   = nil;
    return new;
}
//查找二叉树O(h)
node * tree_search(node *x, int k)
{
    //找到或者到达叶子则返回
    if(x == NULL || x->key == k)
        return x;
    //大于当前节点的值则递归左子树，否则递归右子树
    if(k > x->key){
        tree_search(x->right, k);
    }else{
        tree_search(x->left, k);
    }
}
//非递归方式查找二叉树O(h)
node * iterative_tree_search(node *x, int k)
{
    while(x != NULL && x->key != k){
        if(x->key < k){
            x = x->left;
        }else{
            x = x->right;
        }
    }
    return x;
}
//最小元素O(h)
node * tree_minimum(node *x)
{
    while(x->left != NULL){
        x = x->left;
    }
    return x;
}
//最大元素O(h)
node * tree_maximum(node *x)
{
    while(x->right != NULL){
        x = x->right;
    }
    return x;
}
//前驱O(h)
node * tree_predecessor(node *x)
{
    node *y;
    if(x->left != NULL){
        return tree_maximum(x->left);
    }
    //查找x的最低祖先节点，且y的右孩子是x的祖先
    y = x->p;
    while(y != NULL && x != y->left){
        x = y;
        y = x->p;
    }
    return y;
}
//后继O(h)
node * tree_successor(node *x)
{
    node *y;
    if(x->right != NULL){
        return tree_minimum(x->right);
    }
    //查找x的最低祖先节点，且y的左孩子是x的祖先
    y = x->p;
    while(y != NULL && x != y->right){
        x = y;
        y = x->p;
    }
    return y;
}
//插入O(h)
void tree_insert(tree *T, node *z)
{
    node *x=NULL,*y=NULL;
    x = T->root;
    //在树中查找一个空位置
    while(x != NULL){
        y = x;//记录当前位置
        if(z->key < x->key){
            x = x->left;
        }else{
            x = x->right;
        }
    }
    //插入新节点z
    z->p = y;
    if(y == NULL){
        T->root = z;//T为空树时
    }else{
        if(y->key > z->key){
            y->left = z;
        }else{
            y->right = z;
        }
    }
}
/**
 * 删除有三种情况
 * 1 当节点无子树，直接删除
 * 2 当节点有一个子树，直接删除，在子节点和父节点之间简历一条链
 * 3 当节点有两个子树，找到后继，后继位置执行2操作，用后继替换待删除节点
 */ 
node * tree_delete(tree *T, node *z)
{
    node *x=NULL,*y=NULL;
    //确定三种情况下待删除的y节点
    if(z->left == NULL || z->right == NULL){
        y = z;
    }else{
        y = tree_successor(z);//后继没有左子树(后继定义)
    }
    if(y->left != NULL){
        x = y->left;
    }else{
        x = y->right;
    }
    if(x != NULL){
        x->p = y->p;//把孩子x的p指针指向y节点的父节点
    }
    if(y->p == NULL){
        T->root = x;//当根只有最多一个子节点时，删除
    }else{
        //把y节点的位置替换为孩子x节点
        if(y == y->p->left){
            y->p->left = x;
        }else{
            y->p->right = x;
        }
    }
    //当删除的节点不是真正需要删除的节点时（第三种）
    if(y != z){
        z->key = y->key;//用后继替换z节点的内容
    }
    return y;//返回已经在树中删除的节点
}
//采用二分法（递归）创建
node * tree_create(int *A, int start, int end)
{
    if(start == end)
        return NULL;
    //初始化x
    node *x = new_node();
    //处理节点
    int mid = (start+end)/2;
    x->key = A[mid];
    x->left = tree_create(A, start, mid);
    if(x->left != NULL)
        x->left->p = x;//把子节点的p指针指向T
    x->right = tree_create(A, mid+1, end);
    if(x->right != NULL)
        x->right->p = x;//把子节点的p指针指向T
    return x;
}
//打印树 其中A为一个全为0的辅助矩阵，n是矩阵的列数，m是矩阵的行数
void print_tree(node *x, int (*A)[64], int n, int m, int i)
{
    int j;
    j = x->key;
    A[i][j] = x->key;
    if(x->left != NULL){
        A[i+1][j-1] = -1;
        print_tree(x->left, A, n, m, i+2);
    }
    if(x->right != NULL){
        A[i+1][j+1] = -2;
        print_tree(x->right, A, n, m, i+2);
    }
    if(i == 0){//遍历输出
        for(i = 0; i < n; i++){
            for(j = 0; j < m; j++){
                if(A[i][j] == 0){
                    printf("  ");
                }else if(A[i][j] == -1){
                    printf("/ ");
                }else if(A[i][j] == -2){
                    printf("\\ ");
                }else{
                    printf("%-2d", A[i][j]);
                }
            }
            printf("\n");
        }
    }
}
//中序遍历
int inorder_tree_walk(node *x)
{
    if(x != NULL){
        inorder_tree_walk(x->left);
        printf("%4d", x->key);
        inorder_tree_walk(x->right);
    }
}
//前序遍历
int preorder_tree_walk(node *x)
{
    if(x != NULL){
        printf("%4d", x->key);
        preorder_tree_walk(x->left);
        preorder_tree_walk(x->right);
    }
}
//后序遍历
int postorder_tree_walk(node *x)
{
    if(x != NULL){
        postorder_tree_walk(x->left);
        postorder_tree_walk(x->right);
        printf("%4d", x->key);
    }
}
int main()
{
    tree *T;
    node *x=NULL,*y=NULL,*z=NULL;
    int A[15] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    int B[16][64] = {0};//打印时使用
    int n = 15;
    T = (tree *)malloc(sizeof(tree));
    T->root = tree_create(A, 0, 15);
    print_tree(T->root, B, 7, 64, 0);
    printf("\n----------------------------------分割线-----------------------------------\n");
    //遍历
    printf("中序遍历  ");
    inorder_tree_walk(T->root);
    printf("\n");
    printf("先序遍历  ");
    preorder_tree_walk(T->root);
    printf("\n");
    printf("后序遍历  ");
    postorder_tree_walk(T->root);
    printf("\n----------------------------------分割线-----------------------------------\n");

    x = tree_search(T->root, 12);
    printf("查找 %d\n", x->key);
    y = tree_predecessor(x);
    printf("前驱 %d\n", y->key);
    z = tree_successor(x);
    printf("后继 %d\n", z->key);
    y = tree_minimum(x);
    printf("最小 %d\n", y->key);
    z = tree_maximum(x);
    printf("最大 %d", z->key);

    printf("\n----------------------------------分割线-----------------------------------\n");
    z = new_node();
    z->key = 16;
    printf("插入 z %d\n", z->key);
    tree_insert(T, z);
    print_tree(T->root, B, 9, 64, 0);
    printf("\n----------------------------------分割线-----------------------------------\n");
    y = tree_delete(T, z);
    printf("删除 z %d = y %d\n", z->key, y->key);
    int C[16][64] = {0};
    print_tree(T->root, C, 9, 64, 0);
    return 0;
}

//                 8     
//               /   \   
//         4               12                                                 
//       /   \           /   \                                                
//     2       6       10      14                                             
//   /   \   /   \   /   \   /   \                                            
//   1   3   5   7   9   11  13  15                                           

// ----------------------------------分割线-----------------------------------
// 中序遍历     1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
// 先序遍历     8   4   2   1   3   6   5   7  12  10   9  11  14  13  15
// 后序遍历     1   3   2   5   7   6   4   9  11  10  13  15  14  12   8
// ----------------------------------分割线-----------------------------------
// 查找 12
// 前驱 11
// 后继 13
// 最小 9
// 最大 15
// ----------------------------------分割线-----------------------------------
// 插入 z 16
//                 8     
//               /   \   
//         4               12                                                 
//       /   \           /   \                                                
//     2       6       10      14                                             
//   /   \   /   \   /   \   /   \                                            
//   1   3   5   7   9   11  13  15                                           
//                                 \                                          
//                                 16                                         

// ----------------------------------分割线-----------------------------------
// 删除 z 16 = y 16
//                 8     
//               /   \   
//         4               12                                                 
//       /   \           /   \                                                
//     2       6       10      14                                             
//   /   \   /   \   /   \   /   \                                            
//   1   3   5   7   9   11  13  15                                           