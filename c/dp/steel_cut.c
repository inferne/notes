#include <stdio.h>
#include <malloc.h>

void print_array(int *A, int n)
{
    int i;
    for(i = 0; i <= n; i++){
        printf("%4d", A[i]);
    }
    printf("\n");
}

int steel_cut(int *A, int n)
{
    int *r;
    int *s;
    r = (int *)malloc(sizeof(int) * n);
    s = (int *)malloc(sizeof(int) * n);
    int i,j,q;
    for(i = 1; i <= n; i++){
        q = -1;
        for(j = 1; j <= i; j++){
            if(q < A[j]+r[i-j]){
                q = A[j]+r[i-j];
                s[i] = j;
            }
        }
        r[i] = q;
    }
    printf("%d\n", r[n]);
    for(i = 0; i <= 10; i++){
        printf("%4d", i);
    }
    printf("\n");
    print_array(A, 10);
    print_array(s, n);
    return 1;
}

int main()
{
    int A[11] = {0,1,5,7,11,13,16,16,20,24,30};
    int r;
    steel_cut(A, 7);
    //printf("%d\n", r);
    return 0;
}