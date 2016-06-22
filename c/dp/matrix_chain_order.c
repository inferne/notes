#include <stdio.h>
#include <malloc.h>

void print_array(int *s, int n)
{
    int i,j;
    printf("%8d", 0);
    for (j = 1; j <= n; j++){
        printf("%8d", j);
    }
    printf("\n");
    for(i = 1; i <= n; i++){
        printf("%8d", i);
        for (j = 1; j <= n; j++){
            printf("%8d", *(s + (i-1)*n + j));
        }
        printf("\n");
    }
}

// n个矩阵划分递归式
// p(1) = 1;
// p(n) = p(1)p(n-1)+...+p(k)p(n-k)+...+p(n-1)p(1);
// 最小代价m[i,j]
// m[i,j] = m[i,k] + m[k+1,j] + p(i-1)*p(k)*p(j)
//        = m[i,k] + m[k+1,j] + A[i][0]*A[k+1][0]*A[j][1]
// int matrix_chain_order(int *A, int i, int j){
//     int k;
//     for(k = i; k < j; k++){
//         matrix_chain_order(A, i, k) + matrix_chain_order(A, k+1, j) + A[i][0] * A[k+1][0] * A[j][1];
//     }
// }

// A(1)....A(i)...A(l)....A(k)....A(j)...An
int matrix_chain_order(int (*A)[2], int n)
{
    int *m;
    int l,i,j,k,q;
    int *s;
    m = (int *)malloc(sizeof(int) * n * n);
    s = (int *)malloc(sizeof(int) * n * n);
    for(l = 2; l <= n; l++){// l is the chain length
        //printf("l=%d:\n", l);
        for(i = 1; i <= n - l + 1; i++){
            j = i + l - 1;
            for(k = i; k < j; k++){
                q = *(m + (i-1)*n + k) + *(m + k*n + j) + A[i-1][0] * A[k][0] * A[j-1][1];
                //printf("        q=%d i=%d k=%d j=%d [i,k]=%d [k+1,j]=%d\n", q, i, k, j, *(m + (i-1)*n + k), *(m + k*n + j));
                if(q < *(m + (i-1)*n + j) || *(m + (i-1)*n + j) == 0){
                    *(m + (i-1)*n + j) = q;
                    *(s + (i-1)*n + j) = k;
                }
            }
            //printf("    i=%d j=%d q=%d\n", i, j, *(m + (i-1)*n + j));
        }
        //printf("\n");
    }
    print_array(m, n);
    printf("---------------------------------------------\n");
    print_array(s, n);
}

int main()
{
    int A[6][2] = {{30,35},{35,15},{15,5},{5,10},{10,20},{20,25}};
    matrix_chain_order(A, 6);
    return 0;
}

//        0       1       2       3       4       5       6
//        1       0   15750    7875    9375   11875   15125
//        2       0       0    2625    4375    7125   10500
//        3       0       0       0     750    2500    5375
//        4       0       0       0       0    1000    3500
//        5       0       0       0       0       0    5000
//        6       0       0       0       0       0       0
// ---------------------------------------------
//        0       1       2       3       4       5       6
//        1       0       1       1       3       3       3
//        2       0       0       2       3       3       3
//        3       0       0       0       3       3       3
//        4       0       0       0       0       4       5
//        5       0       0       0       0       0       5
//        6       0       0       0       0       0       0