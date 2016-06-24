#include <stdio.h>
#include <malloc.h>

// void recursive_activity_selector(int *s, int *f, int k, int n)
// {
//     int min = k + 1;
//     while(s[min] < f[k] && min < n){
//         min++;
//     }
//     if(min < n){
//         printf("%4d", f[min]);
//         recursive_activity_selector(s, f, min, n);
//     }else{
//         return ;
//     }
// }

void recursive_activity_selector(int *s, int *f, int k, int n)
{
    int i;
    for(i = 1; i < n; i++){
        if(s[i] > f[k]){
            printf("%4d", f[i]);
            k = i;
        }
    }
}

int main()
{
    int s[12] = {0,1,3,0,5,3,5,6, 8, 8, 2, 12};
    int f[12] = {0,4,5,6,7,9,9,10,11,12,14,16};
    recursive_activity_selector(s, f, 0, 12);
    printf("\n");
    return 0;
}

// 4   7  11  16