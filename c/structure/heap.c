#include <stdio.h>
#include <math.h>

#define LEFT(i) 2*i
#define RIGTH(i) 2*i+1
#define ARRAY_LENGTH(array) (sizeof(array) / sizeof(array[0]))
#define PARENT(i) ceil(i/2)
#define EXCHANGE(a, b) a = a ^ b; b = a ^ b; a = a ^ b;

int * max_heapify(int *A, int i);
int * max_heapify_iterator(int *A, int i);
int * min_heapify(int *A, int i);
int build_max_heap(int *A);
int * heap_sort(int *A);
int heap_maximum(int *A);
int heap_extract_max(int *A);
int heap_increase_key(int *A, int i, int key);
int * max_heap_insert(int *A, int key);

int * heap_size;

int main()
{
    int a[10] = {1,3,5,8,9,2,4,7,6,10};
    int *c;
    int i = 0;
    c = heap_sort(a);
    for(i = 0; i < 10; i++){
        printf("index:%d c:%d\n", i, c[i]);
    }
    return 0;
}

//使i的子树为最大堆
int * max_heapify(int *A, int i)
{
    int l = LEFT(i);
    int r = RIGHT(i);
    int largest;

    if (A[l] > A[i]) {
        largest = l;
    } else {
        largest = i;
    }
    if (A[r] > A[largest]) {
        largest = r;
    }
    if (largest != i) {
        EXCHANGE(A[i], A[largest]);
        max_heapify(A, largest);
    }
    return A;
}

//使i的子树为最大堆（迭代方式实现）
int * max_heapify_iterator(int *A, int i)
{
    while (1) {
        int l = LEFT(i);
        int r = RIGHT(i);
        int largest;

        if (A[l] > A[i]) {
            largest = l;
        } else {
            largest = i;
        }
        if (A[r] > A[largest]) {
            largest = r;
        }
        if (largest != i) {
            EXCHANGE(A[i], A[largest]);
            //max_heapify(A, largest);
            i = largest;
        } else {
            break;
        }
    }
    return A;
}

//使i的子树为最小堆
int * min_heapify(int *A, int i)
{
    int l = LEFT(i);
    int r = RIGHT(i);
    int minimun;

    if (A[l] < A[i]) {
        minimun = l;
    } else {
        minimun = i;
    }
    if (A[r] < A[minimun]) {
        minimun = r;
    }
    if (minimun != i) {
        EXCHANGE(A[i], A[minimun]);
        min_heapify(A, minimun);
    }
    return A;
}

//构造最大堆
int build_max_heap(int *A)
{
    int i;
    int n = ARRAY_LENGTH(A);
    for (i = n/2; i < n; i--){
        max_heapify(A, i);
    }
}

//堆排序
int * heap_sort(int *A)
{
    int i;
    build_max_heap(A);//构造最大堆
    int n = ARRAY_LENGTH(A);
    for (i = n; i > 1; i--) {
        //交换1节点和i节点
        EXCHANGE(A[1], A[i]);
        heap_size[A] = heap_size[A]-1;//去掉i节点
        max_heapify(A, 1);//保持最大堆
    }
}

//返回最大数
int heap_maximum(int *A)
{
    return A[1];
}

//取出最大数
int heap_extract_max(int *A)
{
    int max = 0;
    if (heap_size[A] < 1){
        printf("heap under flow");
        return 0;
    }
    max = A[1];
    A[1] = A[heap_size[A]];
    heap_size[A] = heap_size[A]-1;
    max_heapify(A, 1);
    return max;
}

//增大元素
int heap_increase_key(int *A, int i, int key)
{
    if (key < A[i]){
        printf("new key is smaller than current key");
        return 0;
    }
    A[i] = key;
    int p = PARENT(i);
    while(i > 1 and A[p] < A[i]){
        EXCHANGE(A[i], A[p]);
        i = p;
        p = PARENT(i);
    }
}

//插入元素
int * max_heap_insert(int *A, int key)
{
    heap_size[A] = heap_size[A]+1;
    A[heap_size[A]] = -999999;
    heap_increase_key(A, heap_size[A], key);
}

// //最小堆k路合并
// int min_heap_merge_k(int *A, int *B, int *C)
// {
//     int array[3] = {A[0], B[0], C[0]};
//     build_min_heap(array);
//     //取出最小数
//     min_heapify(array, 1);//最小数对应的数组中的下一个数放入堆中重新生成最小堆
// }

// //构造最小堆
// int build_min_heap(int *array)
// {
//     int i;
//     n = ARRAY_LENGTH(array);
//     for (i = n/2; i > 1; i--){
//         min_heapify(array, i);
//     }
// }
