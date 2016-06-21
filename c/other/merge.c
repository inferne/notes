/* 算法导论2.3.1 合并算法 */
#include <stdio.h>

int main()
{
    int a[5] = {1,3,5,8,9};
    int b[3] = {2,4,7};
    int c[10];
    int i = 0,j = 0;

    while( i < 5 || j < 3 ){
        if( (a[i] <= b[j] && i < 5) || j == 3 ){
            c[i+j] = a[i];
            i++;
        }else{
            c[i+j] = b[j];
            j++;
        }
    }

    int k;
    for(k = 0; k < i+j; k++){
        printf("index:%d c:%d\n", k, c[k]);
    }
    return 0;
}
