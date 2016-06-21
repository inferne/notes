#include <stdio.h>
#include <math.h>

int * merge(int *A, int p, int q, int r);
int * merge_sort(int *A, int p, int r);

int main()
{
    int a[10] = {1,3,5,8,9,2,4,7,6,10};
    int *c;
    int i = 0;
    c = merge_sort(a, 0, 9);
    for(i = 0; i < 10; i++){
        printf("index:%d c:%d\n", i, c[i]);
    }
    return 0;
}
//合并
int * merge(int *A, int start, int middle, int end){
    int L[middle-start+2];
    int R[end-middle+1];
    int i,j,k;
    for (i = 0; i < middle-start+1; i++){
        L[i] = A[start+i];
    }
    for (j = 0; j < end-middle; j++){
        R[j] = A[middle+1+j];
    }
    L[i] = 999999;
    R[j] = 999999;

    i = 0;
    j = 0;
    for (k = start; k <= end; k++) {
        if (L[i] < R[j]){
            A[k] = L[i];
            i++;
        }else{
            A[k] = R[j];
            j++;
        }
        printf("%4d", A[k]);
    }
    printf("\n");
    return A;
}

int * merge_sort(int *A, int start, int end){
    if(start == end) return A;
    int middle = ceil((start+end)/2);

    merge_sort(A, start, middle);
    merge_sort(A, middle+1, end);
    printf("start=%d middle=%d end=%d\n", start, middle, end);
    merge(A, start, middle, end);
    return A;
}
