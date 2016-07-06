#include <stdio.h>
#include <malloc.h>

#define t 2
#define TRUE 1
#define FALSE 0

typedef struct _node node;

struct _node
{
    int   n;
    int   key[3];//节点包含的key
    node  c[4];//节点包含的孩子
    int   leaf;//是否为叶子
};

typedef struct _tree
{
    node root;
} tree;

typedef struct _result
{
    node x;
    int i;
} result;

void disk_read(node x, int i)
{
    //
}

void disk_write(node x, int i)
{
    //
}
//搜索
result b_tree_search(node x, int k)
{
    int i = 1;
    //移动指针到第一个不小于k的位置
    while(i <= x.n && k > x.key[i]){
        i++;
    }
    
    if(k == x.key[i]){//判断等于则返回
        return result ret = {x,i};
    }else if(x.leaf){//叶子节点则返回
        return result ret = {NULL,0};
    }else{//内部节点
        disk_read(x, i);//读取x的c[i]孩子
        b_tree_search(x.c[i], k);//递归查找x.c[i]节点
    }
}
//为新节点分配一个磁盘页
node allocate_node()
{
    node x = (node *)malloc(sizeof(node));
    return x;
}
//分裂x.c[i]节点
void b_tree_split_child(node x, int i)
{
    z = allocate_node();
    node y = x.c[i];
    int j = 0;
    z.leaf = y.leaf;
    z.n = t-1;
    //把y的t+1,...2t-1节点的key给z
    for(j = 1; j < t; j++){
        z.key[j] = y.key[t+j];
    }
    if(!y.leaf){//非叶子节点
        //把y的t+1,...2t的c节点给z
        for(j = 1; j <= t; j++){
            z.c[j] = y.c[t+j];
        }
    }
    y.n = t-1;
    //x节点增加z孩子;
    for(j = x.n+1; j > i; j--){
        x.c[j+1] = x.c[j];
    }
    x.c[i+1] = z;
    //x节点插入y.key[t];
    for(j = x.n; j > i; j--){
        x.key[j+1] = x.key[j];
    }
    x.key[i] = y.key[t];
    x.n = x.n + 1;
    disk_write(x);
    disk_write(y);
    disk_write(z);
}
//插入一个非满的b树
void b_tree_insert_nonfull(node x, int k)
{
    int i = x.n;
    if(x.leaf){
        //找第一个不小于k的数
        while(i > 1 && k < x.key[i]){
            x.key[i+1] = x.key[i];
            i--;
        }
        x.key[i+1] = k;
        x.n = x.n + 1;
        disk_write(x);
    }else{//内部节点
        //找一个x的子节点i插入k
        while(i > 1 && k < x.key[i]){
            i--;
        }
        i++;
        disk_read(x, i);
        if(x.c[i].n == 2*t - 1){//即将插入的子节点满时则分裂
            b_tree_split_child(x, i);
            if(k > x.key[i]){//分裂后和新的i节点比较，大于则i指针加一
                i++;
            }
        }
        b_tree_insert_nonfull(x.c[i], k);//递归插入
    }
}
//插入
void b_tree_insert(tree T, int k)
{
    node r = T.root;
    if(r.n == 2*t - 1){//root满时分裂
        s = allocate_node();
        T.root = s;
        s.n = 0;
        s.leaf = FALSE;
        s.c[1] = r;
        b_tree_split_child(s, 1);
        b_tree_insert_nonfull(s, k);
    }else{
        b_tree_insert_nonfull(s, k);
    }
}
//创建
void b_tree_create(tree T)
{
    x = allocate_node();
    x.leaf = TRUE;
    x.n = 0;
    disk_write(x);
    T.root = x;
}
//合并两个节点
void b_tree_merge_node(node x, node y)
{
    for(j = 1; j <= y.n; j++){
        x.key[x.n+j] = y.key[j];
    }
    if(!x.leaf){
        for(j = 1; j <= y.n+1; j++){
            x.c[x.n+j] = y.c[j];
        }
    }
    x.n += y.n;
    disk_write(x);
}
//直接删除一个key
void b_tree_delete_key(node x, int i)
{
    int j;
    for(j = i; j < x.n; j++){
        x.key[j] = x.key[j+1];
    }
    if(!x.leaf){
        for(j = i+1; j <= x.n; j++){
            x.c[j] = x.c[j+1];
        }
    }
    x.n = x.n - 1;
    disk_write(x);
}
//删除
void b_tree_delete(node x, int k)
{
    int i = 1;
    //在x节点中移动i到一个不小于k的位置
    while(i <= x.n && k < x.key[i]){
        i++;
    }
    if(x.key[i] == k){//k在x中
        if(x.leaf){//x是叶子(情况1)
            //删除key[i]
            b_tree_delete_key(x, i);
        }else{//x非叶子(情况2)
            //至少含有t个关键字(情况2a)
            if(x.c[i].n > t-1){
                //删除x.c[i]中k的前驱,然后用前驱替换k
                b_tree_delete(x.c[i], x.c[i].key[x.n]);
                x.key[i] = x.c[i].key[x.n];
            }
            //x.c[i]含有t-1个关键字,x.c[i+1]至少含有t个关键字(情况2b)
            else if(x.c[i+1].n > t-1){
                //删除x.c[i+1]中k的后继,然后用后继替换k
                b_tree_delete(x.c[i+1], x.c[i+1].key[1]);
                x.key[i] = x.c[i+1].key[1];
            }
            //x.c[i]和x.c[i+1]都含有t-1个关键字(情况2c)
            else{
                //合并x.c[i]和x.c[i+1]
                b_tree_merge_node(x.c[i], x.c[i+1]);
                //删除key[i]
                b_tree_delete_key(x, i);
            }
        }
    }else{//k不在x中(情况3)
        if(x.c[i].n == t-1){//x.c[i]含有t-1个关键字
            //x.c[i]的相邻兄弟节点至少含有t个关键字(情况3a)
            if(x.c[i-1].n > t-1){
                x.c[i].key[1] = x.key[i];//x的key[i]下降到x.c[i][1]
                x.key[i] = x.c[i-1].key[x.c[i-1].n];
                //删除k在左兄弟中的前驱
                b_tree_delete(x.c[i-1], x.c[i-1].key[x.c[i-1].n]);
            }else if(x.c[i+1].n > t-1){
                x.c[i].key[x.c[i].n] = x.key[i];//x的key[i]下降到x.c[i].key[n]
                x.key[i] = x.c[i+1].key[1];
                //删除k在右兄弟中的后继
                b_tree_delete(x.c[i+1], x.c[i+1].key[1]);
            }
            //x.c[i-1]和x.c[i+1]含有t-1个关键字(情况3b)
            else{
                //合并x.c[i-1]和x.c[i]或者x.c[i]和x.c[i+1]
                if(i > 1){//i有左兄弟,则指针前移
                    i = i - 1;
                }
                x.c[i].key[t] = x.key[i];
                x.c[i].n += 1;
                b_tree_merge_node(x.c[i], x.c[i+1]);
                //删除key[i]
                b_tree_delete_key(x, i);
                b_tree_delete(x.c[i], k);
            }
        }else{
            b_tree_delete(x.c[i], k);
        }
    }
}

int main()
{
    char s[] = {'F','S','Q','K','C','L','H','T','V','W','M','R','N','P','A','B','X','Y','D','Z','E'};
    tree T = (tree *)malloc(sizeof(tree));
    b_tree_create(T);
    int i;
    for(i = 0; i < sizeof(s); i++){
        b_tree_insert(T, (int)s[i]);
    }
    return 0;
}