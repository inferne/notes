#include <stdio.h>
#include <malloc.h>

// 设计一个O(n^2)算法，求一个n个数的序列的最长单调递增子序列
// 分析：该问题可看作是一个n序列和自然数1-max(n)的的最长公共子序列问题

void print_lmis(int *A, int *b, int i, int j, int m){
    if(i == 0 || j == 0)
        return ;
    if(*(b + i*m + j) == 1){
        print_lmis(A, b, i-1, j-1, m);
        printf(" %d", A[i]);
    }else if(*(b + i*m + j) == 2){
        print_lmis(A, b, i-1, j, m);
    }else{
        print_lmis(A, b, i, j-1, m);
    }
}

void lmis(int *A, int n)
{
    int i,j,m=0;
    int *c;
    int *b;
    //求序列n的最大数
    for(i = 1; i < n; i++){
        m = m < A[i] ? A[i] : m;
    }

    //lcs算法
    c = (int *)malloc(sizeof(int) * n * (m + 1));
    b = (int *)malloc(sizeof(int) * n * (m + 1));
    for(i = 1; i < n; i++){
        for(j = 1; j <= m; j++){
            if(A[i] == j){
                *(c + i*m + j) = *(c + (i-1)*m + j-1) + 1;
                *(b + i*m + j) = 1;//↖表示指向上一个i和j
            }else if(*(c + (i-1)*m + j) >= *(c + i*m + j-1)){//等于也应该向上
                *(c + i*m + j) = *(c + (i-1)*m + j);
                *(b + i*m + j) = 2;//↑表示指向上一个i
            }else{
                *(c + i*m + j) = *(c + i*m + j-1);
                *(b + i*m + j) = 3;//←表示指向上一个j
            }
            printf(" %d-%d", *(b + i*m + j), *(c + i*m + j));
        }
        printf("\n");
    }
    print_lmis(A, b, n-1, m, m);
    printf("\n");
}

int main()
{
    int A[11] = {0,1,2,4,3,10,5,6,9,7,8};
    lmis(A, 11);
    return 0;
}

 // 1-1 3-1 3-1 3-1 3-1 3-1 3-1 3-1 3-1 3-1
 // 2-1 1-2 3-2 3-2 3-2 3-2 3-2 3-2 3-2 3-2
 // 3-2 2-2 2-2 1-3 3-3 3-3 3-3 3-3 3-3 3-3
 // 3-3 3-3 1-3 2-3 2-3 2-3 2-3 2-3 2-3 2-3
 // 2-3 2-3 2-3 2-3 2-3 2-3 2-3 2-3 2-3 1-4
 // 3-4 3-4 3-4 3-4 1-4 3-4 3-4 3-4 3-4 2-4
 // 2-4 2-4 2-4 2-4 2-4 1-5 3-5 3-5 3-5 3-5
 // 3-5 3-5 3-5 3-5 3-5 2-5 2-5 2-5 1-6 3-6
 // 3-6 3-6 3-6 3-6 3-6 3-6 1-6 3-6 2-6 2-6
 // 2-6 2-6 2-6 2-6 2-6 2-6 2-6 1-7 3-7 3-7
 // 1 2 4 5 6 7 8
