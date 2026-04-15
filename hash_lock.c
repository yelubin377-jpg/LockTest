#include "hash_lock.h"

#include <stdio.h>
#include <stdlib.h>

void hashInit(hash_lock_t* bucket) 
{
  for(int i = 0;i<HASHNUM;i++)
  {
    bucket->table[i].head = NULL;
    pthread_mutex_init(&bucket->table[i].mutex,NULL);
  }
}
int getValue(hash_lock_t* bucket, int key) 
{
  int which = HASH(key);
  pthread_mutex_lock(&bucket->table[which].mutex);
  Hlist p = bucket->table[which].head;   //.h中规定的节点指针，指向链表的头节点
  while(NULL != p)
  {
    if(p->key == key)
    {
      pthread_mutex_unlock(&bucket->table[which].mutex);
      return p->value;
    }
    p = p->next;
  }
  pthread_mutex_unlock(&bucket->table[which].mutex);
  return -1;

}

void insert(hash_lock_t* bucket, int key, int value) 
{
  int which = HASH(key);
  pthread_mutex_lock(&bucket->table[which].mutex);
  Hlist p = bucket->table[which].head;
  while(NULL != p)
  {
    if(p->key == key)
    {
      p->value = value;
      pthread_mutex_unlock(&bucket->table[which].mutex);
      return;
    }
    p=p->next;
  }
  Hlist NewNode = (Hlist)malloc(sizeof(Hnode));
  NewNode->key = key;
  NewNode->value = value;
  NewNode->next = bucket->table[which].head;
  bucket->table[which].head = NewNode;
  pthread_mutex_unlock(&bucket->table[which].mutex);
  return;
  
}

int setKey(hash_lock_t* bucket, int key, int new_key) 
{
  int old_which = HASH(key);
  int new_which = HASH(new_key);
  pthread_mutex_lock(&bucket->table[old_which].mutex);
  Hlist p = bucket->table[old_which].head;
  Hlist former = NULL;//记录前面一个
  while(NULL!=p)
  {
    if(p->key == key)
    {
      if(former == NULL)
      {
        bucket->table[old_which].head = p->next;
      }
      else
      {
        former->next = p->next;
      }
    p->key = new_key;
    p->next = bucket->table[new_which].head;
    bucket->table[new_which].head = p;

    pthread_mutex_unlock(&bucket->table[old_which].mutex);
    return 0;
    }
    former = p;
    p = p->next;
  }
  pthread_mutex_unlock(&bucket->table[old_which].mutex);
  return -1;
}