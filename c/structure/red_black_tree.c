#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

#define red   0
#define black 1

//节点定义
typedef struct _node
{
    struct _node *p;
    struct _node *left;
    struct _node *right;
    int color;
    int key;
} node;

//树定义
typedef struct _tree
{
    node *root;
    node *nil;//哨兵
} tree;

node * new_node()
{
    node *new;
    new = (node *)malloc(sizeof(node));
    new->p = NULL;
    new->left = NULL;
    new->right = NULL;
    new->key = -1;
    return new;
}
//最小节点
node * tree_minimum(tree *T, node *x)
{
    while(x->left != T->nil){
        x = x->left;
    }
    return x;
}
//后继O(h)
node * tree_successor(tree *T, node *x)
{
    node *y;
    if(x->right != T->nil){
        return tree_minimum(T, x->right);
    }
    //查找x的最低祖先节点，且y的左孩子是x的祖先
    y = x->p;
    while(y != T->nil && x != y->right){
        x = y;
        y = x->p;
    }
    return y;
}
//左旋-三条边断裂重连
void left_rotate(tree *T, node *x)
{
    node *y;
    y = x->right;
    //处理x的p节点
    if(x->p == T->nil){
        T->root = y;//当x为根节点时，根节点换为y节点
    }else{
        if(x == x->p->left){
            x->p->left = y;
        }else{
            x->p->right = y;
        }
    }
    y->p = x->p;//y的父节点变成x的父节点
    x->p = y;//x的父节点变成y
    //处理y的左节点
    x->right = y->left;
    if(y->left != T->nil){
        y->left->p = x;
    }
    y->left = x;//y的左节点变成x
}
//右旋-类似左旋
void right_rotate(tree *T, node *y)
{
    node *x;
    x = y->left;
    //处理y的p节点
    if(y->p == T->nil){
        T->root = x;
    }else{
        if(y == y->p->left){
            y->p->left = x;
        }else{
            y->p->right = x;
        }
    }
    x->p = y->p;
    y->p = x;
    //处理x的右节点
    y->left = x->right;
    if(x->right != T->nil){
        x->right->p = y;
    }
    x->right = y;
}
//插入节点后的颜色修正
void rb_insert_fixup(tree *T, node *z)
{
    node *y=T->nil;
    //违反性质4
    while(z->p->color == red){
        if(z->p == z->p->p->left){
            y = z->p->p->right;
            if(y->color == red){//情况1-z的叔父节点y为红色
                z->p->p->color = red;
                z->p->color = black;
                y->color = black;
                z = z->p->p;
            }else if(z == z->p->right){//情况2-z的叔父节点y为黑色，且z是的右孩子
                z = z->p;
                left_rotate(T, z);
            }else{//情况3-z的叔父节点y为黑色，且z是的左孩子
                z->p->p->color = red;
                z->p->color = black;
                right_rotate(T, z->p->p);
            }
        }else{//same as then clause with right and left exchange
            y = z->p->p->left;
            if(y->color == red){//情况1-z的叔父节点y为红色
                z->p->p->color = red;
                z->p->color = black;
                y->color = black;
                z = z->p->p;
            }else if(z == z->p->left){//情况2-z的叔父节点y为黑色，且z是的左孩子
                z = z->p;
                right_rotate(T, z);
            }else{//情况3-z的叔父节点y为黑色，且z是的右孩子
                z->p->p->color = red;
                z->p->color = black;
                left_rotate(T, z->p->p);
            }
        }
    }
    T->root->color = black;//根节点着黑色
}
//插入
void rb_insert(tree *T, node *z)
{
    node *y=T->nil,*x=T->nil;
    x = T->root;
    while(x != T->nil){
        y = x;
        if(z->key < x->key){
            x = x->left;
        }else{
            x = x->right;
        }
    }
    z->p = y;
    if(y == T->nil){
        T->root = z;//当树为空时，z则是根节点
    }else{
        if(z->key < y->key){
            y->left = z;
        }else{
            y->right = z;
        }
    }
    //以上带和和二叉树insert一致
    z->color = red;
    rb_insert_fixup(T, z);//颜色修正
}
//删除节点后颜色修正
void rb_delete_fixup(tree *T, node *x)
{
    node *w;
    while(x != T->root && x->color == black){
        if(x == x->p->left){//当x为父节点的左孩子时
            w = x->p->right;
            if(w->color == red){//情况1-w为红色-转换为情况2/3/4
                w->color = black;
                x->p->color = red;
                left_rotate(T, x->p);
                //w = x->p->right;//新的w节点
            }else if(w->left->color == black && w->right->color == black){//情况2-w为黑色，且w的两个孩子都是黑节点
                w->color = red;
                x = x->p;
            }else if(w->left->color == red && w->right->color == black){//情况3-w为黑色，且左孩子为红色，右孩子为黑色-转换为情况4
                w->color = red;
                w->left->color = black;
                right_rotate(T, w);
                //w = x->p->right;
            }else{//情况4-w为红色，且右孩子是红色
                w->color = x->p->color;
                x->p->color = black;
                w->right->color = black;
                left_rotate(T, x->p);
                x = T->root;//处理完成
            }
        }else{//same as then clause with right and left exchanged
            w = x->p->left;
            if(w->color == red){//情况1-w为红色-转换为情况2/3/4
                w->color = black;
                x->p->color = red;
                right_rotate(T, x->p);
                //w = x->p->left;//新的w节点
            }else if(w->right->color == black && w->left->color == black){//情况2-w为黑色，且w的两个孩子都是黑节点
                w->color = red;
                x = x->p;
            }else if(w->right->color == red && w->left->color == black){//情况3-w为黑色，且左孩子为黑色，右孩子为红色-转换为情况4
                w->color = red;
                w->right->color = black;
                left_rotate(T, w);
                //w = x->p->left;
            }else{//情况4-w为红色，且左孩子是红色
                w->color = x->p->color;
                x->p->color = black;
                w->left->color = black;
                right_rotate(T, x->p);
                x = T->root;//x置为根，while循环退出
            }
        }
    }
    x->color = black;
}
//删除-代码类似二叉树删除代码
node * rb_delete(tree *T, node *z)
{
    node *x=T->nil,*y=T->nil;
    //根据三种情况找到待删除的节点y
    if(z->left == T->nil || z->right == T->nil){
        y = z;
    }else{
        y = tree_successor(T, y);
    }
    //拿到y的子树x
    if(y->left != T->nil){
        x = y->left;
    }else{
        x = y->right;
    }
    x->p = y->p;//y子树x的父节点指向y的父节点
    if(y->p == T->nil){
        T->root = x;//当删除的节点为根时，则把x置为根节点
    }else{
        if(y == y->p->left){
            y->p->left = x;
        }else{
            y->p->right = x;
        }
    }
    if(y != z){
        z->key = y->key;
    }
    if(y->color == black){//当删除的节点为黑节点时红黑树被破坏
        rb_delete_fixup(T, x);
    }
    return y;
}
//创建红黑树
void rb_create(tree *T)
{
    node *z;
    int i,k;
    srand((unsigned)time(NULL));
    for(i = 1; i < 16; i++){
        z = new_node();
        z->p = T->nil;
        z->left = T->nil;
        z->right = T->nil;
        z->key = rand()%1000;//产生0-1000之间的随机数
        rb_insert(T, z);
    }
}
//打印树 其中A为一个全为0的辅助矩阵，n是矩阵的行数，m是矩阵的行数列数
void print_tree(tree *T, node *x, int *A, int n, int m, int i, int j)
{
    //定位j的位置
    if(x == x->p->left){
        j = j - m/(2 << i/2);
    }else{
        j = j + m/(2 << i/2);
    }
    if(x->color == black){
        *(A + (i * m + j))     = -4;
        *(A + (i * m + j + 1)) = x->key;
        *(A + (i * m + j + 2)) = -5;
    }else{
        *(A + (i * m + j + 1)) = x->key;
    }
    if(x->left != T->nil){
        *(A + ((i + 1) * m + j - 1)) = -3;
        print_tree(T, x->left, A, n, m, i+2, j);
    }
    if(x->right != T->nil){
        *(A + ((i + 1) * m + j + 2)) = -2;
        print_tree(T, x->right, A, n, m, i+2, j);
    }
    if(i == 0){//遍历输出
        int k;
        for(i = 0; i < n; i++){
            for(j = 0; j < m; j++){
                k =*(A + (i * m + j)); 
                if(k == 0){
                    printf(" ");
                }else if(k == -3){
                    printf("/");
                }else if(k == -2){
                    printf("\\");
                }else if(k == -4){
                    printf("\b(  ");
                }else if(k == -5){
                    printf(")");
                    j++;
                }else{
                    printf("\b\b%-3d", k);
                }
            }
            printf("\n");
        }
    }
}
//中序遍历
int inorder_tree_walk(tree *T, node *x)
{
    if(x != T->nil){
        inorder_tree_walk(T, x->left);
        printf("%4d", x->key);
        inorder_tree_walk(T, x->right);
    }
}
//前序遍历
int preorder_tree_walk(tree *T, node *x)
{
    if(x != T->nil){
        printf("%4d", x->key);
        preorder_tree_walk(T, x->left);
        preorder_tree_walk(T, x->right);
    }
}
//后序遍历
int postorder_tree_walk(tree *T, node *x)
{
    if(x != T->nil){
        postorder_tree_walk(T, x->left);
        postorder_tree_walk(T, x->right);
        printf("%4d", x->key);
    }
}
int main()
{
    tree *T;
    int n = 16, m = 64;
    int *A = NULL;

    T = (tree *)malloc(sizeof(tree));
    T->nil = new_node();
    T->nil->color = black;
    T->root = T->nil;
    rb_create(T);

    A = (int *)malloc(sizeof(int) * n * m);
    print_tree(T, T->root, A, 10, m, 0, 0);
    printf("\n----------------------------------分割线-----------------------------------\n");
    //遍历
    printf("中序遍历  ");
    inorder_tree_walk(T, T->root);
    printf("\n");
    printf("先序遍历  ");
    preorder_tree_walk(T, T->root);
    printf("\n");
    printf("后序遍历  ");
    postorder_tree_walk(T, T->root);
    printf("\n----------------------------------分割线-----------------------------------\n");

    node *z;
    z = new_node();
    srand((unsigned)time(NULL));
    z->key = rand()%1000;//产生0-1000之间的随机数
    z->p = T->nil;
    z->left = T->nil;
    z->right = T->nil;
    rb_insert(T, z);
    A = (int *)malloc(sizeof(int) * n * m);
    print_tree(T, T->root, A, 10, m, 0, 0);
    printf("\n----------------------------------分割线-----------------------------------\n");

    rb_delete(T, z);
    A = (int *)malloc(sizeof(int) * n * m);
    print_tree(T, T->root, A, 10, m, 0, 0);
    printf("\n----------------------------------分割线-----------------------------------\n");

    return 0;
}

// 输出其中带括号的为黑节点
//                                (776)                            
//                                /  \                             
//                (474)                           (865)            
//                /  \                            /  \             
//        257             (629)           (851)           (898)    
//        /  \            /  \               \            /  \     
//    (231)   (429)   479     693             864     876     903  
//    /                                                            
//  96                                                             
                                                                

// ----------------------------------分割线-----------------------------------
// 中序遍历    96 231 257 429 474 479 629 693 776 851 864 865 876 898 903
// 先序遍历   776 474 257 231  96 429 629 479 693 865 851 864 898 876 903
// 后序遍历    96 231 429 257 479 693 629 474 864 851 876 903 898 865 776
// ----------------------------------分割线-----------------------------------
//                                (776)                            
//                                /  \                             
//                (474)                           (865)            
//                /  \                            /  \             
//        257             (629)           (851)           898      
//        /  \            /  \               \            /  \     
//    (231)   (429)   479     693             864     (876)   (903)
//    /                                               /            
//  96                                              865            
                                                                

// ----------------------------------分割线-----------------------------------
//                                (776)                            
//                                /  \                             
//                (474)                           (865)            
//                /  \                            /  \             
//        257             (629)           (851)           898      
//        /  \            /  \               \            /  \     
//    (231)   (429)   479     693             864     (876)   (903)
//    /                                                            
//  96                                                             
                                                                

// ----------------------------------分割线-----------------------------------
