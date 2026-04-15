#include "list_lock.h"

#include <stdio.h>
#include <stdlib.h>

void listInit(list_lock_t* list) 
{
  list->head = NULL;
  pthread_mutex_init(&list->mutex,NULL);
  pthread_cond_init(&list->cond,NULL);

}

void producer(list_lock_t* list, DataType value) 
{
  pthread_mutex_lock(&list->mutex);//加锁
  LNode *Newnode = (LNode*)malloc(sizeof(LNode));//创建新节点
  Newnode->value = value;//给节点赋值
  Newnode->next = list->head;//头插法
  list->head = Newnode;
  pthread_mutex_unlock(&list->mutex);//解锁
  pthread_cond_signal(&list->cond);//唤醒
}

void consumer(list_lock_t* list) 
{
   pthread_mutex_lock(&list->mutex);//加锁
   while(list->head == NULL)//看看链表是不是空的，空的话就睡觉i
   {
    pthread_cond_wait(&list->cond,&list->mutex);//用conditional Variable来睡觉-->生产者的cond_signal会拍醒消费者
   }
   LNode *CunChu = list->head;
   list->head = CunChu->next;
   free(CunChu);
   pthread_mutex_unlock(&list->mutex);

}

int getListSize(list_lock_t* list) 
{
  pthread_mutex_lock(&list->mutex);//加锁
  int count = 0;//计数
  LNode *p = list->head;//指向头链表，开始走，走一个数一个
  while(p != NULL)
  {
    count++;
    p = p->next;
  }
  pthread_mutex_unlock(&list->mutex);//解锁
  return count;
}
