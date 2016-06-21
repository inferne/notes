#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

//节点定义
typedef struct _node
{
    struct _node *p;
    struct _node *left;
    struct _node *right;
    int hight;//树高
    int key;
} node;

//树定义
typedef struct _tree
{
    node *root;
    node *nil;//哨兵-高度-1
} tree;

node * new_node()
{
    node *new;
    new = (node *)malloc(sizeof(node));
    new->p = NULL;
    new->left = NULL;
    new->right = NULL;
    new->hight = 0;
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
//搜索
node * tree_search(tree *T, node *x, int k)
{
    while(x != T->nil && x->key != k){
        if(k < x->key){
            x = x->left;
        }else{
            x = x->right;
        }
    }
    return x;
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
/**
 * 插入节点后的高度修正
 * 对插入分析得出以下7种情况
 * 1 z == root
 * 2 z != root && z->p == root
 * 3 z != root && z->p != root
 * 4 z 有兄弟节点 不用修改
 * 5 z 无兄弟节点 修改父节点 z上移
 * 6 z->p有兄弟节点
 * 7 z->p无兄弟节点
 */
void avl_insert_fixup(tree *T, node *z)
{
    node *y=T->nil,*w=T->nil;
    //获取z的兄弟w
    if(z == z->p->left){
        w = z->p->right;
    }else{
        w = z->p->left;
    }
    if(w == T->nil && z != T->root){//当z无兄弟节点时，且z不是根节点
        while(z->p != T->root){
            if(z->p == z->p->p->left){//z的p为z的p->p的左节点
                y = z->p->p->right;
                if(z->p->hight > y->hight){//父节点z大与叔父节点y
                    z->p->p->hight = z->p->hight;
                    if(z == z->p->right){//z为右孩子时
                        left_rotate(T, z->p);
                        z->hight++;
                        z = z->left;
                    }else{
                        z->p->hight++;
                    }
                    right_rotate(T, z->p->p);
                    break;
                }else if (z->p->hight < y->hight){//z的p节点高度小于叔父节点高度
                    z->p->hight++;//z的p节点高度增加1
                    break;
                }else{
                    z->p->hight++;//z的p节点高度增加1
                    z = z->p;
                }
            }else{//same as then clause with right and left exchange
                y = z->p->p->left;
                if(z->p->hight > y->hight){//父节点z大与叔父节点y
                    z->p->p->hight = z->p->hight;
                    if(z == z->p->left){//z为左孩子时
                        right_rotate(T, z->p);
                        z->hight++;
                        z = z->right;
                    }else{
                        z->p->hight++;
                    }
                    left_rotate(T, z->p->p);
                    break;
                }else if (z->p->hight < y->hight){//z的p节点高度小于叔父节点高度
                    z->p->hight++;//z的p节点高度增加1
                    break;
                }else{
                    z->p->hight++;//z的p节点高度增加1
                    z = z->p;
                }
            }
        }
        if(z->p == T->root){
            if(z == z->p->left){
                y = z->p->right;
            }else{
                y = z->p->left;
            }
            if(z->hight > y->hight){
                z->p->hight++;//z的p节点高度增加1
            }
        }
    }
}
//插入
void avl_insert(tree *T, node *z)
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
    z->hight = 0;
    avl_insert_fixup(T, z);//高度修正
}
/**
 * 删除节点后高度修正
 删除节点y时的情况分析
 * 1 这种情况会引起z的高度变化，需要递归向上改变各祖先节点的高度
 *         z
 *       /   \
 *     y       w
 *   /
 * x
 * 2 同1，这种情况会引起z的高度变化，需要递归向上改变各祖先节点的高度
 *         z
 *       /   \
 *     y       w
 *       \
 *         x
 * 3 这种情况不会引起不平衡
 *         z
 *       /   \
 *     y       w
 *   /           \
 * x               v
 * 4 这种情况不会引起不平衡
 *         z
 *       /   \
 *     y       w
 *   /       /   \
 * x       u       v
 * 5 这种情况会引起不平衡，需要通过w节点右旋转，u节点再左旋转来调整
 *         z
 *       /   \
 *     y       w
 *   /       /   \
 * x       u       v
 *           \
 *             o
 */
void avl_delete_fixup(tree *T, node *x)
{
    node *w=NULL;
    while(x != T->root){
        if(x == x->p->left){
            w = x->p->right;
            if(x->hight == w->hight){//x节点与兄弟节点w高度相等
                x = x->p;
                x->hight--;//父节点高度减一
            }else if(x->hight + 1 == w->hight){//x的节点比兄弟节点高度少1
                break;//对平衡无影响
            }else if(x->hight + 2 == w->hight){//x的节点高度比兄弟节点少2
                //w节点的左孩子比右孩子高
                if(w->left->hight > w->right->hight){
                    w->hight--;
                    w->left->hight++;
                    right_rotate(T, w);
                    w = w->p;
                }else if(w->right->hight == w->left->hight){
                    w->hight++;
                }
                w->p->hight = w->left->hight+1;
                left_rotate(T, w->p);
                x = x->p->p;
            }
        }else{
            w = x->p->left;
            if(x->hight == w->hight){//x节点与兄弟节点w高度相等
                x = x->p;
                x->hight--;//父节点高度减一
            }else if(x->hight + 1 == w->hight){//x的节点比兄弟节点高度少1
                break;//对平衡无影响
            }else if(x->hight + 2 == w->hight){//x的节点高度比兄弟节点少2
                //w节点的右孩子比左孩子高
                if(w->right->hight > w->left->hight){
                    w->hight--;
                    w->right->hight++;
                    left_rotate(T, w);
                    w = w->p;
                }else if(w->right->hight == w->left->hight){
                    w->hight++;
                }
                w->p->hight = w->right->hight+1;
                right_rotate(T, w->p);
                x = x->p->p;
            }
        }
    }
}
//删除-代码类似二叉树删除代码
node * avl_delete(tree *T, node *z)
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
    avl_delete_fixup(T, x);
    return y;
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
    *(A + (i * m + j)) = x->key;
    *(A + (i * m + j + 1)) = -4;
    *(A + (i * m + j + 2)) = x->hight;
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
        int c = 0;
        for(i = 0; i < n; i++){
            for(j = 0; j < m; j++){
                k = *(A + (i * m + j)); 
                if(k == 0){
                    printf(" ");
                }else if(k == -3){
                    printf("/");
                }else if(k == -2){
                    printf("\\");
                }else if(k == -4){
                    printf("-%d", *(A + (i * m + j + 1)));
                    j++;
                }else{
                    printf("\b\b%-3d", k);
                }
            }
            printf("\n");
        }
    }
}
//创建AVL树
void avl_create(tree *T)
{
    node *z;
    int i,k;
    int B[20] = {566,206,180,133,188,352,315,256,530,804,693,681,776,907,998,122,444,277,666,555};
    for(i = 1; i < 20; i++){
        z = new_node();
        z->p = T->nil;
        z->left = T->nil;
        z->right = T->nil;
        //z->key = rand()%900+100;//产生100-999之间的随机数
        z->key = B[i-1];
        avl_insert(T, z);
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
    T->nil->hight = -1;
    T->root = T->nil;
    srand((unsigned)time(NULL));
    avl_create(T);

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
    int j;
    int C[6] = {122,133,180,188,315,776};
    for(j = 0; j < 6; j++){
        z = tree_search(T, T->root, C[j]);
        avl_delete(T, z);
        A = (int *)malloc(sizeof(int) * n * m);
        printf("delete %d\n", C[j]);
        print_tree(T, T->root, A, 10, m, 0, 0);
        printf("\n----------------------------------分割线-----------------------------------\n");
    }

    return 0;
}

//                               352-4                             
//                                /  \                             
//               206-3                           693-3             
//                /  \                            /  \             
//       180-2           277-1           566-2           804-2     
//        /  \            /  \            /  \            /  \     
//   133-1   188-0   256-0   315-0   530-1   681-1   776-0   907-1 
//    /                               /       /                  \ 
// 122-0                           444-0   666-0               998-0
                                                                

// ----------------------------------分割线-----------------------------------
// 中序遍历   122 133 180 188 206 256 277 315 352 444 530 566 666 681 693 776 804 907 998
// 先序遍历   352 206 180 133 122 188 277 256 315 693 566 530 444 681 666 804 776 907 998
// 后序遍历   122 133 188 180 256 315 277 206 444 530 666 681 566 776 998 907 804 693 352
// ----------------------------------分割线-----------------------------------
// delete 122
//                               352-4                             
//                                /  \                             
//               206-2                           693-3             
//                /  \                            /  \             
//       180-1           277-1           566-2           804-2     
//        /  \            /  \            /  \            /  \     
//   133-0   188-0   256-0   315-0   530-1   681-1   776-0   907-1 
//                                    /       /                  \ 
//                                 444-0   666-0               998-0
                                                                

// ----------------------------------分割线-----------------------------------
// delete 133
//                               352-4                             
//                                /  \                             
//               206-2                           693-3             
//                /  \                            /  \             
//       180-1           277-1           566-2           804-2     
//           \            /  \            /  \            /  \     
//           188-0   256-0   315-0   530-1   681-1   776-0   907-1 
//                                    /       /                  \ 
//                                 444-0   666-0               998-0
                                                                

// ----------------------------------分割线-----------------------------------
// delete 180
//                               352-4                             
//                                /  \                             
//               206-2                           693-3             
//                /  \                            /  \             
//       188-0           277-1           566-2           804-2     
//                        /  \            /  \            /  \     
//                   256-0   315-0   530-1   681-1   776-0   907-1 
//                                    /       /                  \ 
//                                 444-0   666-0               998-0
                                                                

// ----------------------------------分割线-----------------------------------
// delete 188
//                               352-4                             
//                                /  \                             
//               277-2                           693-3             
//                /  \                            /  \             
//       206-1           315-0           566-2           804-2     
//           \                            /  \            /  \     
//           256-0                   530-1   681-1   776-0   907-1 
//                                    /       /                  \ 
//                                 444-0   666-0               998-0
                                                                

// ----------------------------------分割线-----------------------------------
// delete 315
//                               693-4                             
//                                /  \                             
//               352-3                           804-2             
//                /  \                            /  \             
//       256-1           566-2           776-0           907-1     
//        /  \            /  \                               \     
//   206-0   277-0   530-1   681-1                           998-0 
//                    /       /                                    
//                 444-0   666-0                                   
                                                                

// ----------------------------------分割线-----------------------------------
// delete 776
//                               566-3                             
//                                /  \                             
//               352-2                           693-2             
//                /  \                            /  \             
//       256-1           530-1           681-1           907-1     
//        /  \            /               /               /  \     
//   206-0   277-0   444-0           666-0           804-0   998-0 