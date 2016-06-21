#include <stdio.h>
#include <string.h>

int luhn(char *card);

int main(){
    char card[20] = "6212260200046018491";
    int ret;
    //scanf("%s", card);
    printf("user input %s\n", card);
    ret = luhn(card);
    printf("%d\n", ret);
}

int luhn(char *card){
    int total = 0,n,i;
    int len = strlen(card);
    for(i=0; i<len; i++){
        n = *(card+len-i-1)-0x30;
        if(i%2 == 1){
            n = n*2;
            total += n > 10 ? n - 9 : n;
        }else{
            total += n;
        }
        printf("%d ", n);
    }
    printf("\n");
    if(total%10 == 0){
        return 1;
    }else{
        return 0;
    }
}
