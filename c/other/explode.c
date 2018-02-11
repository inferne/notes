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
    char *a = (char *)malloc(sizeof(char)*1024);
    char b[2] = "A";
    for(i = 0; i < str_len; i++){
        while(k > 0 && str[i] != d[k]){
            strncat(a, d, k - next[k-1]);
            k = next[k-1];
        }
        if(str[i] == d[k]){
            k = k +1;
        }else{
            b[0] = str[i];
            strncat(a, b, 1);
            //printf("%c", str[i]);
        }
        if(k == m || i == str_len-1){
            printf("%d %d\n", i, str_len);
            printf("%s\n", a);
            memset(a, '\0', 1024);
            k = next[k];//look for the next match
        }
    }
    //printf("%s\n", a);
}

int main()
{
    char *str = "gjhdsgf\\rhskjdhfkds\\r\\ndsfdsfg";
    char *p = "\\r\\n";
    printf("%s\n", str);
    kmp(str, p);
    return 0;
}

// 1:0 
// gjhdsgf
// hskjdhfkds
// dsfdsfg
