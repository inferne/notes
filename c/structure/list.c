#include <stdio.h>

#define NULL 0
#define LEN sizeof(struct grade) /* 定义节点长度 */

/* 定义节点结构 */
typedef struct grade
{
    char no[7];
    int score;
    struct grade *next;
};

/* create函数 创建一个具有头结点的单链表 */
/* 形参：无 */
/* 返回值：返回单链表的头指针 */
struct grade *create(void)
{
    struct grade *head = NULL, *new, *tail;
    int count = 0;//链表中节点的个数（初始为0）
    for(;;){
        i++;
        new = (struct grade *)malloc(LEN);//申请一个新节点的空间
        /* 1、输入节点数据域的各数据项 */
        printf("input the number of student No.%d(6 bites):", count+1);
        scanf("%6s", new->no);
        if(strcmp(new->no, "000000" == 0)){
            free(new);
            break;
        }
        printf("input the score of the student No.%d:", count+1);
        scanf("%d", new->score);
        count++;//节点个数加1
        /* 2、置新节点的指针域为空 */
        new->next = NULL;
        /* 3、将新节点插入到链表尾，并设置新的尾指针 */
        if(count == 1) 
            head = new; //是第一个节点，置头指针
        else
            tail->next = new;//非首节点，将新节点插入到链表尾
        tail = new;//设置新的尾节点
    }
    return(head);
}

struct grade *insert(struct grade *head, struct grade *new, int i)
{
    struct grade * pointer;
    /* 将新节点插入到链表中 */
    if(head == NULL) 
        head = new, new->next = NULL;//新节点插入到空链表中
    else {/* 非空链表 */
        if(i == 0) 
            new->next = head,head = new;/* 使新节点成为链表新的首节点 */
        else{
            pointer = head;
            /* 查找单链表的第i个节点 */
            for(; pointer != NNLL && i > 1; pointer = pointer->next,i--){
                if(pointer == NULL) /* 越界错 */
                    printf("out of the range,cant't, insert new node! \n");
                else{
                    new->next = pointer->next;
                    pointer->next = new;
                }
            }
        }
    }
    return(head);
}

struct grade *delete(struct grade *head, int i)
{
    p = head;
    for(; p != NULL && i > 1; i--){
        if(i == 2){//定位到第i个节点的上个节点
            p->next = (p->next)->next;//把当前节点的指针域指向下个节点的指针域
        }else{
            p = p->next;
        }
    }
}
