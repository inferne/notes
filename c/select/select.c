/* 算法导论9.3-最坏情况线性时间的选择 */
#include <stdio.h>

#define exchange(a,b) if(a != b){ a = a ^ b; b = a ^ b; a = a ^ b; };

int insert_sort(int *A, int start, int length);
int partition(int *A, int start, int end);
int average(int *A, int start, int end, int p);
int find_middle(int *A, int start, int end, int p);
int select_x(int *A, int start, int end);
int select(int *A, int start, int end, int i);
int var_dump(int *A, int start, int end);

int main()
{
    int A[19] = {12,15,36,84,54,21,64,97,87,85,82,83,86,14,13,18,16,17,19};
    int i=10;//第i小的数
    int j;
    j = select(A, 0, sizeof(A)/sizeof(int)-1, i-1);
    printf("%dth:%d\n", i, j);
    
    return 0;
}

//插入排序
int insert_sort(int *A, int start, int length)
{
    int i,j;
    for(i = start; i < start+length; i++){
        for (j = i; j > start; j--){
            if(A[j] < A[j-1]){
                exchange(A[j], A[j-1]);
            }
        }
    }
    return 1;
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
    printf("partition:%d-%d-------------------------------------\n", start, end);
    var_dump(A, start, end);
    return i;
}

//将输入数组的n个元素划分为n/p组并排序
int average(int *A, int start, int end, int p)
{
    int i;
    //按p把数组划分为n/p个子数组
    for(i = start; i <= end; i+=p){
        if(i + p > end){
            insert_sort(A, i, end-i+1);
        }else{
            insert_sort(A, i, p);//对每个子数组进行插入排序
        }
    }
    printf("按p把数组划分为n/p个子数组-------------------------------------\n");
    var_dump(A, start, end);
}

//找出划分后每组的中位数
int find_middle(int *A, int start, int end, int p)
{
    int i;
    int j = start;
    //把中位数依次放到数组的前面
    for(i = start; i <= end; i+=p){
        if(i+p > end){
            exchange(A[j], A[(i+end+1)/2]);
        }else{
            exchange(A[j], A[(i+i+p)/2]);
        }
        j++;
    }
    printf("\n把中位数依次放到数组的前面-------------------------------------\n");
    var_dump(A, start, end);
    return j-1;
}

//找出n/p个中位数的中位数x
int select_x(int *A, int start, int end)
{
    //当范围内只有一个元素时返回
    if(start == end)
        return A[start];
    int p = 5;
    int j;
    average(A, start, end, p);
    j = find_middle(A, start, end, p);
    //对上面找到的中位数，调用select，找到他们的中位数x
    select_x(A, start, j);
}
//
int select(int *A, int start, int end, int i)
{
    int j,k;
    select_x(A, start, end);
    printf("找到x:%d-------------------------------------\n", A[start]);
    var_dump(A, start, end);
    //然后按x对数组进行划分，另k比小于x的元数数目多一，x即为第k小的数，n-k个数为比x大的数
    exchange(A[start], A[end]);
    k = partition(A, start, end);
    //如果i=k，则返回x。否则，如果i<k，则在低区递归调用select以找出第i小的元素，如果i>k，则在高区找第(i-k)个最小元素
    if(i == k)
        return A[k];
    if(i < k){
        select(A, start, k-1, i);
    }else{
        select(A, k+1, end, i);
    }
}

int var_dump(int *A, int start, int end)
{
    int i;
    for (i = start; i <= end; i++){
        printf("%4d", A[i]);
    }
    printf("\n");
}

//程序输出

// 按p把数组划分为n/p个子数组-------------------------------------
//   12  15  36  54  84  21  64  85  87  97  13  14  82  83  86  16  17  18  19

// 把中位数依次放到数组的前面-------------------------------------
//   36  85  82  18  84  21  64  15  87  97  13  14  12  83  86  16  17  54  19
// 按p把数组划分为n/p个子数组-------------------------------------
//   18  36  82  85

// 把中位数依次放到数组的前面-------------------------------------
//   82  36  18  85
// 找到x:82-------------------------------------
//   82  36  18  85  84  21  64  15  87  97  13  14  12  83  86  16  17  54  19
// partition:0-18-------------------------------------
//   19  36  18  21  64  15  13  14  12  16  17  54  82  83  86  97  84  85  87
// 按p把数组划分为n/p个子数组-------------------------------------
//   18  19  21  36  64  12  13  14  15  16  17  54

// 把中位数依次放到数组的前面-------------------------------------
//   21  14  54  36  64  12  13  19  15  16  17  18
// 按p把数组划分为n/p个子数组-------------------------------------
//   14  21  54

// 把中位数依次放到数组的前面-------------------------------------
//   21  14  54
// 找到x:21-------------------------------------
//   21  14  54  36  64  12  13  19  15  16  17  18
// partition:0-11-------------------------------------
//   18  14  12  13  19  15  16  17  21  36  64  54
// 按p把数组划分为n/p个子数组-------------------------------------
//   36  54  64

// 把中位数依次放到数组的前面-------------------------------------
//   54  36  64
// 找到x:54-------------------------------------
//   54  36  64
// partition:9-11-------------------------------------
//   36  54  64
// 找到x:36-------------------------------------
//   36
// partition:9-9-------------------------------------
//   36
// 10th:36
