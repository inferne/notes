#include <stdio.h>
#include <malloc.h>

// 求一个n个数的最长单调递增子序列
// 设序列x有n个数
// 序列m=<1,2...k>为最长单调递增子序列
// 则有m(1)<=m(2)<=...<=m(k)
// c[i]表示x中前i个数的单调递增子序列m=<1,2,...j>
// 则有
// c[i] = c[i-1]            x(i) < m(j)
// c[i] = c[i-1] + 1        x(i) >= m(j)
// 需要记住c[i-1]中的最后一个元素m(j)
// 如果出现多个一样长度的子序列，则取第一个子序列为最优，因为第一个的子序列的尾元素最小，可以产生最长单调递增子序列
// 时间复杂度O(n^2)

void print_lmis(int *b, int k, int i)
{
    if(k == 0) return ;
    //输出以b[k]为最大数的序列中的元素
    if(i > b[k-1]){
        print_lmis(b, k-1, b[k-1]);
        printf("%2d", i);
    }else{
        print_lmis(b, k-1, i);
    }
}

void lmis(int *A, int n)
{
    int *c;//记录子序列中的个数
    int *b;//记录子序列中的最大值
    int i,j;
    c = (int *)malloc(sizeof(int) * n);
    b = (int *)malloc(sizeof(int) * n);
    for(i = 1; i < n; i++){
        printf("%2d ", A[i]);
        for(j = 1; j <= i; j++){
            if(A[i] >= *(b + j-1)){
                *(c + i) = *(c + j-1) + 1;
                *(b + i) = A[i];
            }
            //当序列c[i]和c[j-1]相等时，优先保留当前序列c[i]，因为当前序列的尾元素会比j-1序列小
            else if(*(c + i) < *(c + j-1)){
                *(c + i) = *(c + j-1);
                *(b + i) = *(b + j-1);
            }
            printf(" %2d-%d", *(b + i), *(c + i));
        }
        printf("\n");
    }

    print_lmis(b, n-1, b[n-1]);
    printf("\n");
}

// 一个长度为i的候选子序列尾元素至少不比长度为i-1的候选子序列的尾元素小
// 每输入一个元素A[i]，如果A[i]不小于M中的最大元素，则把A[i]追加到M中
// 否则替换M中大于a的数中最小元素，替换的时候采用二分查找
// 时间复杂度降低为O(nlgn)
void lmis_nlgn(int *A, int n)
{
    int *M;
    int i,j=0,k;
    M = (int *)malloc(sizeof(int) * n);
    for(i = 1; i < n; i++){
        if(A[i] >= M[j]){
            j++;
            M[j] = A[i];
        }else{
            //二分查找m
            k = (1 + j)/2;
            while(M[k-1] > A[i] || M[k] < A[i]){
                if(M[k] > A[i]){
                    k = k/2;
                }else{
                    k = k + k/2;
                }
            }
            M[k] = A[i];//找到大于A[i]的最小M[k]
        }
    }
    for(i = 1; i <= j; i++){
        printf("%2d", M[i]);
    }
    printf("\n");
}
// 输出: 1 2 3 5 6 7 8

int main()
{
    int A[11] = {0,1,2,4,3,10,5,6,9,7,8};//0为哨兵，不参数计算
    lmis(A, 11);
    lmis_nlgn(A, 11);
    return 0;
}

// lmis(A,11)
//  1   1-1
//  2   2-1  2-2
//  4   4-1  4-2  4-3
//  3   3-1  3-2  3-3  3-3
// 10  10-1 10-2 10-3 10-4 10-4
//  5   5-1  5-2  5-3  5-4  5-4  5-4
//  6   6-1  6-2  6-3  6-4  6-4  6-4  6-5
//  9   9-1  9-2  9-3  9-4  9-4  9-4  9-5  9-6
//  7   7-1  7-2  7-3  7-4  7-4  7-4  7-5  7-6  7-6
//  8   8-1  8-2  8-3  8-4  8-4  8-4  8-5  8-6  8-6  8-7
//  1 2 3 5 6 7 8
