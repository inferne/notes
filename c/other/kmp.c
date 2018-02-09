#include <stdio.h>
#include <malloc.h>
#include <string.h>

int * next_prifix(char *p)
{
    size_t m = strlen(p);
    int *next = (int *)malloc(sizeof(int) * m);

    int i, k = 0;
    next[0] = 0;
    for(i = 1; i < m; i++){
        while(k > 0 && p[i] != p[k]){
            k = next[k-1];
        }
        if(p[i] == p[k]){
            k = k + 1;
        }
        next[i] = k;
        printf("%d:%d ", i, next[i]);
    }
    printf("\n");
    return next;
}
/**
 * kmp算法分割字符串
 */
void kmp(char *str, char *d)
{
    int str_len = strlen(str);

    int m = strlen(d);
    int *next = next_prifix(d);
    int i = 0, k = 0;
    for(i = 0; i < str_len; i++){
        while(k > 0 && str[i] != d[k]){
            k = next[k-1];
        }
        if(str[i] == d[k]){
            k = k +1;
        }else{
            printf("%c", str[i]);
        }
        if(k == m){
            printf("\n");
        }
        //k = next[k];
    }
    printf("\n");
}

int main()
{
    char *str = "gjhdsgf\r\nhskjdhfkds\r\ndsfdsfg";
    char *p = "\r\n";
    kmp(str, p);
    return 0;
}

// 1:0 
// gjhdsgf
// hskjdhfkds
// dsfdsfg
