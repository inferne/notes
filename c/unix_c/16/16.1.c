#include <stdio.h>

int main()
{
    int x = 0x12345678;

    printf("[0]:0x%x\n", *((char *)&x + 0));
    printf("[1]:0x%x\n", *((char *)&x + 1));
    printf("[2]:0x%x\n", *((char *)&x + 2));
    printf("[3]:0x%x\n", *((char *)&x + 3));

    return 0;
}

// [0]:0x78
// [1]:0x56
// [2]:0x34
// [3]:0x12
