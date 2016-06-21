/* 算法导论8.4-桶排序 */
#include <stdio.h>
#include <math.h>

int insert_list(int *l, int *e);
int insert_sort(int *A);
int bucket_sort(int *A, int length);

typedef struct _bucket
{
    int value;
    char *next;
} bucket;

int main()
{
    int A[10] = {52,22,17,28,34,44,97,23,54,87};
    int i;
    bucket_sort(A, sizeof(A)/sizeof(int));
    for(i = 0; i < 10; i++){
        printf("index:%d A:%d\n", i, A[i]);
    }
    return 0;
}

int bucket_sort(int *A, int length)
{
    int i,j,*bt;
    int *B[length];//指针数组
    //把A中的元素放入捅中
    for(i = 0; i < length; i++){
        bt = (bucket *)malloc(sizeof(bucket));
        bt->value = A[i];
        insert_list(B[A[i]/10], bt);
    }
    //对每个捅中元素进行插入排序
    for(i = 0; i < length; ++i){
        insert_sort(B[i]);
    }
    //合并所有桶中的元素
    j = 0;
    for(i = 0; i < length; i++){
        bt = B[i];
        do{
            bt = bt->next;
            A[j] = bt->value;
            j++;
        } while (bt->next != NULL);
    }
}
