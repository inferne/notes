#include <stdio.h>
#include <math.h>
#include <malloc.h>

int high(x)
{
    return x;
}

int low(x)
{
    return x;
}

int index(x)
{
    return x;
}

typedef struct _proto_node proto_node;

struct _proto_node
{
    int          u;
    proto_node  *summary;
    proto_node  *cluster;
    int          A[2];
};

/* 查找 T(u) = T(√u) + O(1) => T(u) = O(lglgu)*/
int proto_veb_member(proto_node *V, int x)
{
    if(V->u == 2){
        return V->A[x];
    }else{
        return proto_veb_member(V->cluster[high(x)], low(x));
    }
}

/* 最小值 T(u) = 2T(√u) + O(1) => T(u) = O(lgu)*/
int proto_veb_minimum(proto_node *V)
{
    if(V->u == 2){
        if(V->A[0] == 1){
            return 0;
        }else if(V->A[1] == 1){
            return 1;
        }else{
            return NULL;
        }
    } else {
        min-cluster = proto_veb_minimum(V->summary);
        if(min-cluster == NULL)
            return NULL;
        else {
            offset = proto_veb_minimum(V->cluster[min-cluster]);
            return index(min-cluster, offset);
        }
    }
}

/* 查找后继 T(u) = 2T(√u) + O(lg√u) = 2T(√u) + O(lgu) => T(u) = lgulglgu*/
int proto_veb_successor(proto_node *V, int x)
{
    if(V->u == 2){
        if(x == 0 and V->A[1] == 1)
            return 1;
        else
            return NULL;
    } else {
        offset = proto_veb_successor(V->cluster[high(x)], low(x));
        if(offset != NULL)
            return index(high(x), offset);
        else {
            succ-cluster = proto_veb_successor(V->summary, high(x));
            if(succ-cluster == NULL)
                return NULL;
            else {
                offset = proto_veb_minimum(V->cluster[succ-cluster]);
                return index(succ-cluster, offset);
            }
        }
    }
}

/* 插入元素  T(u) = 2T(√u) + O(1) => T(u) = O(lgu)*/
int proto_veb_insert(proto_node *V, int x)
{
    if(V->u == 2)
        V->A[x] = 1;
    else {
        proto_veb_insert(V->cluster[high(x)], low(x));
        proto_veb_insert(V->summary, high(x));
    }
}

/**************************van Emde Boas***************************/

typedef struct _node node;

struct _node
{
    int   u;
    int   min;
    int   max;
    node *summary;
    node *cluster;
}

/* O(1) */
int veb_tree_minimum(node *V)
{
    return V->min;
}

/* O(1) */
int veb_tree_maximum(node *V)
{
    return V->max;
}

/* O(lglgu) */
int veb_tree_member(node *V, int x)
{
    if(x == V->min or x == V->max){
        return TRUE;
    }else if(V->u == 2){
        return FALSE;
    }else{
        return veb_tree_member(V->cluster[high(x)], low(x));
    }
}

/* 查找后继 O(lglgu) */
int veb_tree_successor(node *V, int x)
{
    if(V->u == 2){
        if(x == 0 and V->max == 1){
            return 1;
        }else{
            return NULL;
        }
    }else if(V->min != NULL and x < V->min){
        return V->min;
    }else{
        //取x族节点中的最大元素
        max_low = veb_tree_maximum(V->cluster[high(x)]);
        //x族节点中的最大元素与低位比较
        if(max_low != NULL and low(x) < max_low){
            //取低位在x族中后继
            offset = veb_tree_successor(V->cluster[high(x)], low(x));
            //返回后继
            return index(high(x), offset);
        }else{
            //从summary查找高位的后继族
            succ-cluster = veb_tree_successor(V->summary, high(x));
            if(succ-cluster == NULL){
                return NULL;
            }else{
                //获取后继族中的最小元素
                offset = veb_tree_minimum(V->cluster[succ-cluster]);
                return index(succ-cluster, offset);
            }
        }
    }
}

/* 查找前驱 O(lglgu) */
int veb_tree_predecessor(node *V, int x)
{
    if(V->u == 2){
        if(x == 1 and V->min == 0){
            return 0;
        }else{
            return NULL;
        }
    }else if(V->max != NULL and x > V->max){
        return V->max;
    }else{
        //取x族节点中的最小元素
        min_low = veb_tree_minimum(V->cluster[high(x)]);
        //x族节点中的最小元素与低位比较
        if(min_low != NULL and low(x) > min_low){
            //取低位在x族中前驱
            offset = veb_tree_predecessor(V->cluster[high(x)], low(x));
            //返回前驱
            return index(high(x), offset);
        }else{
            //从summary查找高位的前驱族
            pred-cluster = veb_tree_predecessor(V->summary, high(x));
            if(pred-cluster == NULL){
                if(V->min != NULL and x > V->min)
                    return V->min;
                else
                    return NULL;
            }else{
                offset = veb_tree_maximum(V->cluster[pred-cluster]);
                return index(pred-cluster, offset);
            }
        }
    }
}

void veb_empty_tree_insert(node *V, int x)
{
    V->min = x;
    V->max = x;
}

/* 插入语 O(lglgu) */
void veb_tree_insert(node *V, int x)
{
    if(V->min == NULL){
        veb_empty_tree_insert(V, x);
    }else{
        if(x < V->min){
            exchange(x, V->min);
        }
        if(V->u > 2){
            //判断x族是否为空
            if(veb_tree_minimum(V->cluster[high(x)]) == NULL){
                veb_tree_insert(V->summary, high(x));
                veb_empty_tree_insert(V->cluster[high(x)], low(x));
            }else{
                veb_tree_insert(V->cluster[high(x)], low(x));
            }
        }
        if(x > V->max){
            V->max = x;
        }
    }
}

/* 删除 O(lglgu) */
void veb_tree_delete(node *V, int x)
{
    if(V->min == V->max){
        V->min = NULL;
        V->max = NULL;
    }else if(V->u == 2){
        if(x == 0){
            V->min = 1;
        }else{
            V->min = 0;
            V->max = V->min;
        }
    }else if(x == V->min){
        first-cluster = veb_tree_minimum(V->summary);
        x = index(first-cluster, veb_tree_minimum(V->cluster[first-cluster]));
        V->min = x;
        veb_tree_delete(V->cluster[high(x)], low(x));
        //判断删除后的x族是否变为空
        if(veb_tree_minimum(V->cluster[high(x)]) == NULL){
            //从summary中移除x族号
            veb_tree_delete(V->summary, high(x));
            //判断删除的是否为最大元素
            if(x == V->max){
                summary-max = veb_tree_maximum(V->summary);
                if(summary-max == NULL){
                    v->max = V->min;
                }else{
                    V->max = index(summary-max, veb_tree_maximum(V->cluster[summary-max]));
                }
            }
        }else if(x == V->max){
            V->max == index(high(x), veb_tree_maximum(V->cluster[high(x)]));
        }
    }
}