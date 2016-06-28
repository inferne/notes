#include <stdio.h>
#include <malloc.h>

typedef struct _bucket Bucket;

struct _bucket
{
    int   val;
    int   h;
    int   key;
    Bucket *next;
};

typedef struct _hash_table
{
    int nTableMask;
    Bucket *data;
    int nTableSize;//表大小
    int nNumUsed;//使用的
} HashTable;

int hash(int mask, int key)
{
    return key & mask;
}
//动态表实现
HashTable * set(HashTable *ht, int key, int val)
{
    Bucket *b;
    b = (Bucket *)malloc(sizeof(Bucket));
    //check hash table size
    if(ht->nTableSize == 0){ //init
        ht->data = (Bucket *)malloc(sizeof(Bucket));
        ht->nTableSize += 1;
    }else if(ht->nTableSize == ht->nNumUsed){
        //new a hash table
        HashTable *nht;
        nht = (HashTable *)malloc(sizeof(HashTable));
        nht->data = (Bucket *)malloc(sizeof(Bucket) * (ht->nTableSize * 2));
        nht->nTableSize = ht->nTableSize * 2;
        nht->nTableMask = ht->nTableSize * 2 - 1;
        //move old table to new table
        int i;
        Bucket *v;
        v = (Bucket *)malloc(sizeof(Bucket));
        for(i = 0; i < ht->nNumUsed; i++){
            v->val = ht->data[i].val;
            v->key = ht->data[i].key;
            v->h = hash(nht->nTableMask, v->key);
            nht->data[v->key] = *v;
        }
        nht->nNumUsed = ht->nNumUsed;
        ht = nht;//replace old table
    }
    //set
    b->h = hash(ht->nTableMask, key);
    b->val = val;
    b->key = key;
    if(ht->data[b->h].val == 0){
        ht->data[b->h] = *b;
    }else{
        ht->data[b->h].next = b;
    }
    ht->nNumUsed++;
    return ht;
}

int get(HashTable *ht, int key)
{
    int h = hash(ht->nTableMask, key);
    int val = 0;
    Bucket *b = &ht->data[h];
    while(b != NULL && val == 0){
        if(b->key == key){
            val = b->val;
        }else{
            b = b->next;
        }
    }
    return val;
}

void delete(HashTable *ht, int key)
{
    int h = hash(ht->nTableMask, key);
    int val = 0;
    Bucket *b = &ht->data[h];
    while(b != NULL && val == 0){
        if(b->key == key){
            if(b->next != NULL){
                b->val = b->next->val;
                b->h = b->next->h;
                b->key = b->next->key;
            }else{
                b->val = 0;
                b->h = 0;
                b->key = 0;
            }
            ht->nNumUsed--;
        }else{
            b = b->next;
        }
    }
}

int main()
{
    HashTable *ht;
    int i;
    for(i = 0; i < 10; i++){
        ht = set(ht, i, i+100);
    }
    for(i = 0; i < 10; i++){
        printf("%4d", get(ht, i));
    }
    printf("\n");
    delete(ht, 4);
    delete(ht, 6);
    for(i = 0; i < 10; i++){
        printf("%4d", get(ht, i));
    }
    printf("\n");
    return 0;
}

// 100 101 102 103 104 105 106 107 108 109
// 100 101 102 103   0 105   0 107 108 109