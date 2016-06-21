#include <stdio.h>
#include <malloc.h>

#define TRUE 1
#define FALSE 0

typedef struct _stack
{
    int top;//栈顶指针
    int A[16];//数组指针
} stack;

int stack_empty(stack *S)
{
    if(S->top){
        return TRUE;
    }else{
        return FALSE;
    }
}

int push(stack *S, int x)
{
    S->top = S->top+1;
    S->A[S->top] = x;
    printf("top %d,push %d\n", S->top, S->A[S->top]);
    return S->A[S->top];
}

int pop(stack *S)
{
    if(stack_empty(S) == 0){
        printf("underflow\n");
        return -1;
    }else{
        S->top = S->top-1;
    }
    printf("top %d,pop %d\n", S->top+1,S->A[S->top+1]);
    return S->A[S->top+1];
}

int main()
{
    stack *S;
    S = (stack *)malloc(sizeof(stack));
    //init
    S->top = 0;

    push(S, 3);
    push(S, 2);
    push(S, 4);
    pop(S);
    pop(S);
    pop(S);
    pop(S);
    
    return 0;
}
