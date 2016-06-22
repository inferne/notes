#include <stdio.h>
#include <string.h>
#include <malloc.h>

//最长公共子序列
// int lcs_length(char *X, char *Y)
// {
//     int c;
//     int m = strlen(X);
//     int n = strlen(Y);
//     if(m == n == 0){
//         c = 0;
//     }else if(X[m] == Y[n]){
//         c += lcs(strncpy(X, X, m-1), strncpy(Y, Y, n-1)) + 1;
//     }else{
//         c += max(lcs(X, strncpy(Y, Y, n-1)), lcs(strncpy(X, X, m-1), Y));
//     }
//     return c;
// }

void print_lcs(int *b, char *X, int i, int j, int n)
{
    if(i == 0 || j == 0){
        return ;
    }else if(*(b + i*n + j) == 1){
        print_lcs(b, X, i-1, j-1, n);
        printf("%c", X[i]);
    }else if(*(b + i*n + j) == 2){
        print_lcs(b, X, i-1, j, n);
    }else{
        print_lcs(b, X, i, j-1, n);
    }
}

void lcs_length(char *X, char *Y)
{
    int *c;
    int *b;
    int m = strlen(X);
    int n = strlen(Y);
    c = (int *)malloc(sizeof(int) * m * n);
    b = (int *)malloc(sizeof(int) * m * n);
    int i,j;
    printf(" ");
    for(j = 0; j < n; j++){
        printf("%4c", Y[j]);
    }
    printf("\n%c", X[0]);
    for(i = 0; i < n; i++){
        printf("%4d", 0);
    }
    printf("\n");
    for(i = 1; i < m; i++){
        printf("%c%4d", X[i], 0);
        for(j = 1; j < n; j++){
            if(X[i] == Y[j]){
                *(c + i*n + j) = *(c + (i-1)*n + (j-1)) + 1;
                *(b + i*n + j) = 1;
            }else if(*(c + (i-1)*n + j) >= *(c + i*n + j-1)){
                *(c + i*n + j) = *(c + (i-1)*n + j);
                *(b + i*n + j) = 2;
            }else{
                *(c + i*n + j) = *(c + i*n + j-1);
                *(b + i*n + j) = 3;
            }
            printf(" %d-%d", *(b + i*n + j), *(c + i*n + j));
        }
        printf("\n");
    }
    print_lcs(b, X, i-1, j-1, n);
    printf("\n");
}

int main()
{
    char *X;
    char *Y;
    X = "xABCBDAB";
    Y = "yBDCABA";
    int c = 0;
    lcs_length(X, Y);
    return 0;
}

//     y   B   D   C   A   B   A
// x   0   0   0   0   0   0   0
// A   0 2-0 2-0 2-0 1-1 3-1 1-1
// B   0 1-1 3-1 3-1 2-1 1-2 3-2
// C   0 2-1 2-1 1-2 3-2 2-2 2-2
// B   0 1-1 2-1 2-2 2-2 1-3 3-3
// D   0 2-1 1-2 2-2 2-2 2-3 2-3
// A   0 2-1 2-2 2-2 1-3 2-3 1-4
// B   0 1-1 2-2 2-2 2-3 1-4 2-4
// BCBA
