#include <stdio.h>
#include <malloc.h>

void print_array(float *s, int n)
{
    int i,j;
    printf("%8d", 0);
    for (j = 0; j < n; j++){
        printf("%8d", j);
    }
    printf("\n");
    for(i = 1; i <= n; i++){
        printf("%8d", i);
        for (j = 0; j < n; j++){
            printf("%8.2f", *(s + i*n + j));
        }
        printf("\n");
    }
}

void optimal_bst(float *p, float *q, int n)
{
    float *e;
    float *w;
    float *root;
    float t;
    int l,i,j,r;

    e    = (float *)malloc(sizeof(float) * n * n);
    w    = (float *)malloc(sizeof(float) * n * n);
    root = (float *)malloc(sizeof(float) * n * n);
    
    for(i = 1; i <= n; i++){
        *(e + i*n + i-1) = q[i-1];
        *(w + i*n + i-1) = q[i-1];
    }

    for(l = 1; l < n; l++){
        for(i = 1; i < n-l+1; i++){
            j = i + l -1;
            *(w + i*n + j) = *(w + i*n + j-1) + p[j] + q[j];
            for(r = i; r <= j; r++){
                t = *(e + i*n + r-1) + *(e + (r+1)*n + j) + *(w + i*n + j);
                if(t < *(e + i*n + j) || *(e + i*n + j) == 0.00){
                    *(e + i*n + j) = t;
                    *(root + i*n + j) = r;
                }
            }
            //printf("i=%d j=%d w=%-8.2fe=%-8.2froot=%-8.2f\n", i, j,*(w + i*n + j), *(e + i*n + j), *(root + i*n + j));
        }
    }

    print_array(e, n);
    print_array(w, n);
    print_array(root, n);
}

int main()
{
    float p[6] = {0,    0.15, 0.10, 0.05, 0.10, 0.20};
    float q[6] = {0.05, 0.10, 0.05, 0.05, 0.05, 0.10};
    optimal_bst(p, q, 6);
    return 0;
}

// 0       0       1       2       3       4       5
// 1    0.05    0.45    0.90    1.25    1.75    2.75
// 2    0.00    0.10    0.40    0.70    1.20    2.00
// 3    0.00    0.00    0.05    0.25    0.60    1.30
// 4    0.00    0.00    0.00    0.05    0.30    0.90
// 5    0.00    0.00    0.00    0.00    0.05    0.50
// 6    0.00    0.00    0.00    0.00    0.00    0.10

// 0       0       1       2       3       4       5
// 1    0.05    0.30    0.45    0.55    0.70    1.00
// 2    0.00    0.10    0.25    0.35    0.50    0.80
// 3    0.00    0.00    0.05    0.15    0.30    0.60
// 4    0.00    0.00    0.00    0.05    0.20    0.50
// 5    0.00    0.00    0.00    0.00    0.05    0.35
// 6    0.00    0.00    0.00    0.00    0.00    0.10

// 0       0       1       2       3       4       5
// 1    0.00    1.00    1.00    2.00    2.00    2.00
// 2    0.00    0.00    2.00    2.00    2.00    4.00
// 3    0.00    0.00    0.00    3.00    4.00    5.00
// 4    0.00    0.00    0.00    0.00    4.00    5.00
// 5    0.00    0.00    0.00    0.00    0.00    5.00
// 6    0.00    0.00    0.00    0.00    0.00    0.00
