#include <stdio.h>
#include <malloc.h>

#define TRUE 1
#define FALSE 0

static int t = 2;

typedef struct _node node;

struct _node
{
    int    n;
    int   *key;//节点包含的key
    node  *c;//节点包含的孩子
    int    leaf;//是否为叶子
};

typedef struct _tree
{
    node *root;
} tree;

typedef struct _result
{
    node *x;
    int i;
} result;

void disk_read(node *x, int i)
{
    //
}

void disk_write(node *x)
{
    //
}
print_node(node *x)
{
    printf("n=%-3dkey1=%-3dkey2=%-3dkey3=%-3dc1=%p c2=%p c3=%p leaf=%d\n", x->n, x->key[1], x->key[2], x->key[3], x->c+1, x->c+2, x->c+3, x->leaf);
}
//搜索
result b_tree_search(node *x, int k)
{
    int i = 1;
    //移动指针到第一个不小于k的位置
    while(i <= x->n && k > x->key[i]){
        i++;
    }
    
    if(k == x->key[i]){//判断等于则返回
        result ret = {x,i};
        return ret;
    }else if(x->leaf){//叶子节点则返回
        result ret;
        return ret;
    }else{//内部节点
        disk_read(x, i);//读取x的c[i]孩子
        b_tree_search(x->c+i, k);//递归查找x->c+i节点
    }
}
//为新节点分配一个磁盘页
node * allocate_node()
{
    node *x = (node *)malloc(sizeof(node));
    x->key = (int *)malloc(sizeof(int) * 2 * t);//key分配内存
    x->c = (node *)malloc(sizeof(node) * (2 * t + 1));//孩子节点分配内存
    return x;
}
//分裂x->c+i节点
void b_tree_split_child(node *x, int i)
{
    node *z = allocate_node();
    node *y = x->c+i;
    int j = 0;
    z->leaf = y->leaf;
    z->n = t-1;
    //把y的t+1,...2t-1节点的key给z
    for(j = 1; j < t; j++){
        z->key[j] = y->key[t+j];
    }
    if(!y->leaf){//非叶子节点
        //把y的t+1,...2t的c节点给z
        for(j = 1; j <= t; j++){
            *(z->c+j) = *(y->c+t+j);
        }
    }
    y->n = t-1;
    //x节点增加z孩子;
    for(j = x->n+1; j > i; j--){
        *(x->c+j+1) = *(x->c+j);
    }
    *(x->c+i+1) = *z;
    //x节点插入y->key[t];
    for(j = x->n; j >= i; j--){
        x->key[j+1] = x->key[j];
    }
    x->key[i] = y->key[t];
    x->n = x->n + 1;
    // printf("\nsplit i x:");
    // print_node(x);
    // printf("y:");
    // print_node(y);
    // printf("z:");
    // print_node(z);
    disk_write(x);
    disk_write(y);
    disk_write(z);
}
//插入一个非满的b树
void b_tree_insert_nonfull(node *x, int k)
{
    int i = x->n;
    if(x->leaf){
        //找第一个不小于k的数
        while(i >= 1 && k < x->key[i]){
            x->key[i+1] = x->key[i];
            i--;
        }
        x->key[i+1] = k;
        x->n = x->n + 1;
        disk_write(x);
        // print_node(x);
    }else{//内部节点
        //找一个x的子节点i插入k
        while(i >= 1 && k < x->key[i]){
            i--;
        }
        i++;
        disk_read(x, i);
        if((x->c+i)->n == 2*t - 1){//即将插入的子节点满时则分裂
            b_tree_split_child(x, i);
            if(k > x->key[i]){//分裂后和新的i节点比较，大于则i指针加一
                i++;
            }
        }
        b_tree_insert_nonfull(x->c+i, k);//递归插入
    }
}
//插入
void b_tree_insert(tree *T, int k)
{
    node *r = T->root;
    if(r->n == 2*t - 1){//root满时分裂
        node *s = allocate_node();
        T->root = s;
        s->n = 0;
        s->leaf = FALSE;
        *(s->c+1) = *r;
        b_tree_split_child(s, 1);
        b_tree_insert_nonfull(s, k);
    }else{
        b_tree_insert_nonfull(r, k);
    }
}
//创建
void b_tree_create(tree *T)
{
    node *x = allocate_node();
    x->leaf = TRUE;
    x->n = 0;
    disk_write(x);
    T->root = x;
    // print_node(T->root);
}
//合并两个节点
void b_tree_merge_node(node *x, node *y)
{
    int j;
    for(j = 1; j <= y->n; j++){
        x->key[x->n+j] = y->key[j];
    }
    if(!x->leaf){
        for(j = 1; j <= y->n+1; j++){
            *(x->c+x->n+j) = *(y->c+j);
        }
    }
    x->n += y->n;
    disk_write(x);
}
//直接删除一个key
void b_tree_delete_key(node *x, int i)
{
    int j;
    for(j = i; j < x->n; j++){
        x->key[j] = x->key[j+1];
    }
    if(!x->leaf){
        for(j = i+1; j <= x->n; j++){
            *(x->c+j) = *(x->c+j+1);
        }
    }
    x->n = x->n - 1;
    disk_write(x);
}
//删除
void b_tree_delete(tree *T, node *x, int k)
{
    int i = 1;
    //在x节点中移动i到一个不小于k的位置
    while(i <= x->n && x->key[i] < k){
        i++;
    }
    if(x->key[i] == k){//k在x中
        if(x->leaf){//x是叶子(情况1)
            //删除key[i]
            b_tree_delete_key(x, i);
        }else{//x非叶子(情况2)
            disk_read(x, i);
            disk_read(x, i+1);
            //至少含有t个关键字(情况2a)
            if((x->c+i)->n > t-1){
                //删除x->c+i中k的前驱,用前驱替换k
                x->key[i] = (x->c+i)->key[(x->c+i)->n];
                b_tree_delete(T, x->c+i, (x->c+i)->key[(x->c+i)->n]);
            }
            //x->c+i含有t-1个关键字,x->c[i+1]至少含有t个关键字(情况2b)
            else if((x->c+i+1)->n > t-1){
                //删除x->c[i+1]中k的后继,用后继替换k
                x->key[i] = (x->c+i+1)->key[1];
                b_tree_delete(T, x->c+i+1, (x->c+i+1)->key[1]);
            }
            //x->c+i和x->c[i+1]都含有t-1个关键字(情况2c)
            else{
                //合并x->c+i和x->c[i+1]
                b_tree_merge_node(x->c+i, x->c+i+1);
                //删除key[i]
                b_tree_delete_key(x, i);
            }
        }
    }else if(!x->leaf){//x不是叶子节点且k不在x中(情况3)
        disk_read(x, i);
        if((x->c+i)->n == t-1){//x->c+i含有t-1个关键字
            disk_read(x, i-1);
            disk_read(x, i+1);
            //x->c+i的相邻兄弟节点至少含有t个关键字(情况3a)
            if((x->c+i-1)->n >= t){
                //检查k是否在x->c+i-1中
                //x的key[i-1]下降到x->c+i[1]
                int j;
                for(j = (x->c+i)->n; j >= 1; j--){
                    (x->c+i)->key[j+1] = (x->c+i)->key[j];
                }
                (x->c+i)->key[j+1] = x->key[i-1];
                x->key[i-1] = (x->c+i-1)->key[(x->c+i-1)->n];
                if(!(x->c+i)->leaf){
                    for(j = (x->c+i)->n+1; j >= 1; j--){
                        *((x->c+i)->c+j+1) = *((x->c+i)->c+j);
                    }
                    //把x->c+i+1的最后一个孩子给x->c+i
                    *((x->c+i)->c+1) = *((x->c+i-1)->c+(x->c+i-1)->n+1);
                }
                (x->c+i)->n += 1;
                //删除k在左兄弟中的前驱
                b_tree_delete_key(x->c+i-1, (x->c+i-1)->n);
            }else if((x->c+i+1)->n >= t && i <= x->n){//i <= x->n条件避免使用已删除的节点
                //x的key[i]下降到(x->c+i)->key[n]
                (x->c+i)->key[(x->c+i)->n+1] = x->key[i];
                x->key[i] = (x->c+i+1)->key[1];
                if(!(x->c+i)->leaf){
                    //把x->c+i+1的第一个孩子给x->c+i
                    *((x->c+i)->c+(x->c+i)->n+2) = *((x->c+i+1)->c+1);
                }
                (x->c+i)->n += 1;
                //删除k在右兄弟中的后继
                *((x->c+i+1)->c+1) = *((x->c+i+1)->c+2);
                b_tree_delete_key(x->c+i+1, 1);
            }
            //x->c[i-1]和x->c[i+1]含有t-1个关键字(情况3b)
            else{
                //合并x->c[i-1]和x->c+i或者x->c+i和x->c[i+1]
                if(i > 1){//i有左兄弟,则指针前移
                    i = i - 1;
                }
                (x->c+i)->key[t] = x->key[i];
                (x->c+i)->n += 1;
                b_tree_merge_node(x->c+i, x->c+i+1);
                b_tree_delete_key(x, i);
                //当根节点被合并且为空节点
                if(x == T->root && x->n == 0){
                    T->root = x->c+i;
                }
            }
        }
        b_tree_delete(T, x->c+i, k);
    }
}
/**
 * 等分原理每个节点由父节点绝对占的线段，然后在这段线中间输出节点数据
 * --------1--------
 * ----1---1---1----
 * --1-1---1-1-1-1--
 */
void print_btree(node *x, int *A, int n, int m, int i, int s, int e)
{
    int j = s + (e - s)/2;//j定位到中间
    int k;
    for(k = 1; k <= x->n; k++){
        *(A + (i * m + (j - x->n/2 + k - 1))) = x->key[k];
    }
    if(!x->leaf){
        int b = (e - s)/(x->n+1);//把j-s的线段等分
        s = s - b;
        for(k = 1; k <= x->n+1; k++){
            s = s + b;
            e = s + b;
            if((s + e)/2 < j){
                *(A + ((i + 1) * m + ((s + e)/2 + j)/2)) = -2;
            }else if((s + e)/2 == j){
                *(A + ((i + 1) * m + ((s + e)/2 + j)/2)) = -3;
            }else{
                *(A + ((i + 1) * m + ((s + e)/2 + j)/2)) = -4;
            }
            print_btree(x->c+k, A, n, m, i+2, s, e);
            //print_btree(x->c+k, A, n, m, i+2, s+(k-1)*b, s+k*b);
        }
    }
    if(i == 0){//遍历输出
        int k;
        for(i = 0; i < n; i++){
            for(j = 0; j < m; j++){
                k =*(A + (i * m + j)); 
                if(k == 0){
                    printf(" ");
                }else if(k == -2){
                    printf("/");
                }else if(k == -3){
                    printf("|");
                }else if(k == -4){
                    printf("\\");
                }else{
                    printf("%c", k);
                }
            }
            printf("\n");
        }
    }
}
int main()
{
    char s[] = "FSQKCLHTVWMRNPABXYDZE";
    char d[] = "DTBCMP";
    tree *T = (tree *)malloc(sizeof(tree));
    b_tree_create(T);
    int i;
    printf("create tree\n");
    for(i = 0; i < sizeof(s) - 1; i++){
        // printf("insert:%-4d", (int)s[i]);
        b_tree_insert(T, (int)s[i]);
    }
    int *A;
    int n = 5,m = 64;
    A = (int *)malloc(sizeof(int) * n * m);
    print_btree(T->root, A, n, m, 0, 0, m);
    
    for(i = 0; i < sizeof(d) - 1; i++){
        printf("--------------------------------------------------------------------\n");
        printf("delete:%c\n", d[i]);
        b_tree_delete(T, T->root, (int)d[i]);
        A = (int *)malloc(sizeof(int) * n * m);
        print_btree(T->root, A, n, m, 0, 0, m);
    }
    printf("\n");
    return 0;
}

// create tree
//                                KQ                               
//                      /         /          \                     
//          BF                    M                   TW           
//       /   |  \              /    \              /   |  \        
//    A     CDE     H        L        NP       RS      V     XYZ   
// --------------------------------------------------------------------
// delete:D    情况1-D在叶子上
//                                KQ                               
//                      /         /          \                     
//          BF                    M                   TW           
//       /   |  \              /    \              /   |  \        
//    A     CE      H        L        NP       RS      V     XYZ   
// --------------------------------------------------------------------
// delete:T    情况2a-T在内部节点上，且左孩子的n>t
//                                KQ                               
//                      /         /          \                     
//          BF                    M                   SW           
//       /   |  \              /    \              /   |  \        
//    A     CE      H        L        NP        R      V     XYZ   
// --------------------------------------------------------------------
// delete:B    情况2b-B在内部节点上，且右孩子的n>t
//                                KQ                               
//                      /         /          \                     
//          CF                    M                   SW           
//       /   |  \              /    \              /   |  \        
//    A      E      H        L        NP        R      V     XYZ   
// --------------------------------------------------------------------
// delete:C    情况2c-C在内部节点上，且左右孩子n=t-1
//                                KQ                               
//                      /         /          \                     
//           F                    M                   SW           
//        /    \               /    \              /   |  \        
//     AE         H          L        NP        R      V     XYZ   
// --------------------------------------------------------------------  
// delete:M    情况3a-M在内部节点上，且右兄弟n>=t
//                                KS                               
//                      /         /          \                     
//           F                   NQ                    W           
//        /    \              /   |  \              /    \         
//     AE         H        L      P      R        V        XYZ     
// --------------------------------------------------------------------
// delete:P    情况3b-P在页节点上，且右兄弟n=t-1
//                                KS                               
//                      /         /          \                     
//           F                    Q                    W           
//        /    \               /    \               /    \         
//     AE         H         LN         R          V        XYZ     
