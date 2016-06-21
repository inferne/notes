#include <stdio.h>
#include <math.h>

int counting_sort(int *A, int *B, int length, int k, int digit);
int radix_sort(int *A, int length, int d, int k);

int main()
{
    int A[10] = {523,224,178,289,345,448,978,230,548,877};
    int length=10;//数组总数
    int d=3;//位数
    int k=10;//进制
    int i;
    radix_sort(A, length, d, k);
    for(i = 0; i < 10; i++){
        printf("index:%d A:%d\n", i, A[i]);
    }
    return 0;
}

int counting_sort(int *A, int *B, int length, int k, int digit)
{
    int C[k];
    int i,j;

    //init C[];
    for(i = 0; i < k; i++){
        C[i] = 0;
    }
    //A[j]中等于i的元素个数
    for(j = 0; j < length; j++){
        C[(A[j]/digit)%10] += 1;
    }
    //A[j]中小于等于i的元素个数
    for (i = 1; i < k; i++){
        C[i] += C[i-1];
    }
    //输出排序数组B
    for (j = length-1; j >= 0; j--){
        B[C[(A[j]/digit)%10]-1] = A[j];
        C[(A[j]/digit)%10]--;
    }
    printf("%4d%4d%4d%4d%4d%4d%4d%4d%4d%4d\n", B[0],B[1],B[2],B[3],B[4],B[5],B[6],B[7],B[8],B[9]);
    return 1;
}

int radix_sort(int *A, int length, int d, int k)
{
    int B[length];
    int i,j;
    int digit=1;
    for(i = 1; i <= d; i++){
        counting_sort(A, B, length, k, digit);
        for(j = 0; j < length; j++)
        {
            A[j] = B[j];
        }
        printf("-------------------------------\n");
        digit *= k;
    }
}

//(a / x ) % y

//a / 1 %10
//a / 10 %10
//a / 100 % 10

//  230 523 224 345 877 178 448 978 548 289
// -------------------------------
//  523 224 230 345 448 548 877 178 978 289
// -------------------------------
//  178 224 230 289 345 448 523 548 877 978
// -------------------------------
// index:0 A:178
// index:1 A:224
// index:2 A:230
// index:3 A:289
// index:4 A:345
// index:5 A:448
// index:6 A:523
// index:7 A:548
// index:8 A:877
// index:9 A:978
