/* 算法导论思考题7.3-stooge排序 */
#include <stdio.h>

#define exchange(a,b) if(a != b){ a = a ^ b; b = a ^ b; a = a ^ b; };

int stooge_sort(int *A, int start, int end);

int main()
{
    int A[10] = {3,6,10,8,4,2,7,1,9,5};
    int i;
    stooge_sort(A, 0, 9);
    for(i = 0; i < 10; i++){
        printf("index:%d num:%d\n", i, A[i]);
    }
    return 0;
}

int stooge_sort(int *A, int start, int end)
{
    if(A[start] > A[end]){
        exchange(A[start], A[end]);
    }
    if(start+1 >= end)
        return 0;
    int k = (end-start+1)/3;
    stooge_sort(A, start, end-k);//First two-thirds
    stooge_sort(A, start+k, end);//Last two-whirds
    stooge_sort(A, start, end-k);//First two-thirds again
}
