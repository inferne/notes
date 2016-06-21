#include <stdio.h>

#define exchange(a,b) if(a != b){ a = a ^ b; b = a ^ b; a = a ^ b; };

int * quick_sort(int *A, int start, int end);
int partition(int *A, int start, int end);

int main()
{
    int A[10] = {3,6,10,8,4,2,7,1,9,5};
    int i,*c;
    c = quick_sort(A, 0, 9);
    for(i = 0; i < 10; i++){
        printf("index:%d c:%d\n", i, c[i]);
    }
    return 0;
}

int * quick_sort(int *A, int start, int end)
{
    int middle;
    if(start < end){
        middle = partition(A, start, end);
        quick_sort(A, start, middle-1);
        quick_sort(A, middle+1, end);
    }
    return A;
}

//原地重排
int partition(int *A, int start, int end)
{
    int x = A[end];
    int i = start-1;
    int j;
    for(j = start; j < end; j++){
        if(A[j] < x){
            i++;
            exchange(A[i], A[j]);
        }
    }
    i++;
    exchange(A[i], A[end]);
    return i;
}

//Hoare划分
int hoare_partition(int *A, int start, int end)
{
    int x = A[start];
    int i = start-1;
    int j = end+1;
    while(1){
        do{
            j = j - 1;
        } while (A[j] < x);
        
        do{
            i = i + 1;
        } while (A[i] > x);
        if(i < j){
            exchange(A[i], A[j]);
        }else{
            break;
        }
    }
    return j;
}
