#include <stdio.h>

#define exchange(a,b) if(a != b){ a = a ^ b; b = a ^ b; a = a ^ b; };

int partition(int *A, int start, int end);
int randomized_select(int *A, int start, int end, int i);

int main()
{
    int A[10] = {3,6,10,8,4,2,7,1,9,5};
    int i,c;
    c = randomized_select(A, 0, 9, 6);
    printf("c:%d\n", c);
    
    return 0;
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

int randomized_select(int *A, int start, int end, int i)
{
    if(start == end)
        return A[start];
    int q,k;
    q = partition(A, start, end);
    k = q - start + 1;
    if(i == k)
        return A[q];
    if(i < k)
        randomized_select(A, start, q-1, i);
    else
        randomized_select(A, q+1, end, i-k);
}
