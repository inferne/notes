#include <stdio.h>

#define LIST_INIT_SIZE 100 //线性表存储空间的初始分配量
#define LISTINCREMENT  10  //线性表存储空间的分配增量

typedef struct {
    ElemType *elem;        //存储空间基地址
    int      length;       //当前长度
    int      listsize;     //当前分配的存储容量（以sizeof(ElemType)为单位）
}SqList;

Status InitList_Sq(SqList &L)
{
    //构造一个空的线性表L
    L.elem = (ElemType *)malloc(LIST_INIT_SIZE * sizeof(ElemTYpe));
    if(!L.elem) exit(OVERFLOW);
    L.length = 0;
    L.listsize = LIST_INIT_SIZE;
    return OK;
}

Status ListInsert_Sq(SqList &L, int i, ElemType e)
{
    //在顺序线性表L中第i个位置之前插入新的元素e
    //i的合法值为1<=i<=ListLength_Sq(L)+1
    if(i < 1 || i > L.length + 1) return ERROR;//i值不合法
    if(L.length >=  L.listsize){//当前存储空间已满，增加分配
        newbase = (ElemType *)realloc(L.elem, (L.listsize + LISTINCREMENT) * sizeof(ElemType));
        if(!newbase) exit(OVERFLOW);//存储分配失效
        L.elem = newbase;//新基地址
        L.listsize += LISTINCREMENT;//增加存储容量
    }
    q = &(L.elem[i-1]);//q为插入位置
    for(p = &(L.elem[L.length - 1]); p >= q; --p)
        *(p+1) = *p;//插入位置之后的元素右移
    *q = e;//插入e
    ++L.length;//表长增1
    return OK;
}

Status ListDelete_Sq(SqList &L, int i, ElemType &e)
{
    //在线性表上删除第i个元素，并用e返回其值
    //i的合法值为1<=i<=ListLength_Sq(L)
    if(i < 1 || i > L.length) return ERROR;
    p = &(L.elem[i-1]);//被删除元素的位置
    e = *p;//被删除元素的值赋给e
    q = L.elem + L.length - 1;//表尾元素的位置
    for( ++p; p <= q; ++p )//被删除元素之后的元素上移
        *(p-1) = *p;
    --L.length;//表长减一
    return OK;
}

//书中方法
Status LocateElem_Sq(SqList L, ElemType e, Status(*compare)(ElemType, ElemType))
{
    //在顺序线性表中查找第一个与e满足compare()的元素
    i = 1;
    p = L.elem;
    while( i <= L.length && !(*compare)(*p++, e) )
        ++i;
    if(i <= L.length) 
        return i;
    else 
        return 0;
}

//自写方法
Status LocateElem_Sq_My(SqList L, ElemType e, Status(*compare)(ElemType, ElemType))
{
    //在顺序线性表中查找第一个与e满足compare()的元素
    p = L.elem;
    for(i = 1; i <= L.length; i++){
        if( (*compare)(*p++,e) ){
            return i;
        }
    }
    return 0;
}

void MergeList_Sq(SqList La, SqList Lb, SqList &Lc)
{
    //已知顺序线性表La和Lb的元素按值非递减排列
    //归并La和Lb得到新的线性表Lc，Lc的元素也按值非递减排列
    pa = La.elem;
    pb = Lb.elem;
    Lc.listsize = Lc.length =  La.length + Lb.length;
    pc = Lc.elem = malloc(Lc.listsize * sizeof(ElemType));
    pa_last = La.elem + La.length - 1;
    pb_last = Lb.elem + Lb.length - 1;
    while(pa <= pa_last || pb <= pb_last){
        if(*pa >= *pb){
            *pc++ = *pa++;
        }else{
            *pc++ = *pb++;
        }
    }
}
