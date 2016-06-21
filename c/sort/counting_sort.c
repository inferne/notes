/* 算法导论8.2-计数排序 */
#include <stdio.h>

int counting_sort(int *A, int *B, int length, int k);

int main()
{
    int A[10] = {3,6,10,8,4,2,7,1,9,5};
    int B[10];
    int i;
    counting_sort(A, B, 10, 10);
    for(i = 0; i < 10; i++){
        printf("index:%d B:%d\n", i, B[i]);
    }
    return 0;
}

int counting_sort(int *A, int *B, int length, int k)
{
    int C[k];
    int i,j;

    //init C[];
    for(i = 0; i <= k; i++){
        C[i] = 0;
    }
    //A[j]中等于i的元素个数
    for(j = 0; j < length; j++){
        C[A[j]] += 1;
    }
    //A[j]中小于等于i的元素个数
    for (i = 1; i <= k; i++){
        C[i] += C[i-1];
    }
    //输出排序数组B
    for (j = length-1; j >= 0; j--){
        B[C[A[j]]-1] = A[j];
        C[A[j]]--;
    }
    return 1;
}
