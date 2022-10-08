#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "concurrent_list.h"

struct node {
  // add more fields
  int value;
  struct node *next;
  pthread_mutex_t mutex;
}; 

struct list {
  // add fields
  struct node *head;
  pthread_mutex_t mutex;
};

void print_node(node* node)
{
  // DO NOT DELETE
  if(node)
  {
    printf("%d ", node->value);
  }
}



list* create_list()
{
  list *l=(list*)malloc(sizeof(list));
  l->head=NULL;
  pthread_mutex_init(&(l->mutex),NULL);
  return l;
}


void delete_list(list* list){ 
   if(!list){
	   return;
   }

  pthread_mutex_lock(&(list->mutex));

  node *curr=list->head;

  if(!curr){
    pthread_mutex_unlock(&(list->mutex));
    pthread_mutex_destroy(&(list->mutex));
    free(list);
  }

  pthread_mutex_lock(&(curr->mutex));

  if(!curr->next){
    pthread_mutex_unlock(&(curr->mutex));
    pthread_mutex_destroy(&(curr->mutex));
    free(curr);
  }else{
    while(curr->next){
	    pthread_mutex_lock(&(curr->next->mutex));
      curr=curr->next;
    }

    while(list->head){
      curr=list->head;
      list->head=curr->next;

      pthread_mutex_unlock(&(curr->mutex));
      pthread_mutex_destroy(&(curr->mutex));
      free(curr);
    }
  }
  pthread_mutex_unlock(&(list->mutex));
  pthread_mutex_destroy(&(list->mutex));
  free(list);
  return;
}



void insert_value(list* list, int value){
    if(list==NULL){
      return;
    }

    node *curr = list->head;
    node* tmp_node=(node*)malloc(sizeof(node));
    if(!tmp_node){
      return;
    }else{
      pthread_mutex_init(&(tmp_node->mutex),NULL);
      tmp_node->value=value;
      tmp_node->next=NULL; 
    }

    if (curr==NULL) {
        list->head=tmp_node;
        pthread_mutex_unlock(&(tmp_node->mutex));
        return;
    }else{pthread_mutex_lock(&(curr->mutex));}

    

    if(curr->value >= value){
      tmp_node->next=curr;
      list->head=tmp_node;
      pthread_mutex_unlock(&(curr->mutex));
      return;
    }

    node *prev = curr;
    while (curr->next && curr->next->value < value) {
		    
        prev = curr;
        curr = curr->next;
        pthread_mutex_lock(&(curr->mutex));
        pthread_mutex_unlock(&(prev->mutex));
	}

  if(curr->next==NULL){
    curr->next=tmp_node;
    pthread_mutex_unlock(&(curr->mutex));
    return;

  }

    tmp_node->next=curr->next;
    curr->next=tmp_node;
    pthread_mutex_unlock(&(curr->mutex));
    return;
}

void remove_value(struct list* list, int value)
{
  if (!list){
	  return;
  }
  node* prev=list->head;
  if (!prev){
	  return;
  }else{
    pthread_mutex_lock(&(prev->mutex));
  }

  if(prev->value==value){
    list->head=prev->next;
    pthread_mutex_unlock(&(prev->mutex));
	  pthread_mutex_destroy(&(prev->mutex));
    free(prev);
    return;
  } 
   
  if(prev->next){
    node* curr=prev->next;
    pthread_mutex_lock(&curr->mutex);

    if(curr->value==value){
      prev->next=curr->next;
      pthread_mutex_unlock(&(prev->mutex));
      pthread_mutex_unlock(&(curr->mutex));
		  pthread_mutex_destroy(&curr->mutex);
	    free(curr);
      return;
    }

    while(curr->next && curr->value <= value){
      if(curr->value==value){
        prev->next=curr->next;
        pthread_mutex_unlock(&(prev->mutex));
        pthread_mutex_unlock(&(curr->mutex));
		    pthread_mutex_destroy(&curr->mutex);
	      free(curr);
        return;
      }
      pthread_mutex_unlock(&(prev->mutex));
      prev=curr;
      curr=curr->next;
      pthread_mutex_lock(&(curr->mutex));
    }
    if(curr->value==value){
        prev->next=curr->next;
        pthread_mutex_unlock(&(prev->mutex));
        pthread_mutex_unlock(&(curr->mutex));
		    pthread_mutex_destroy(&curr->mutex);
	      free(curr);
        return;
    }
    pthread_mutex_unlock(&(curr->mutex));
  }
  pthread_mutex_unlock(&(prev->mutex));
  return;
 }
 


void print_list(list* list)
{
  // add code here
if(list && list->head){
  node *prev = list->head;
  pthread_mutex_lock(&(prev->mutex));

  

  if(!prev->next){
    print_node(prev);
    pthread_mutex_unlock(&(prev->mutex));
    printf("\n"); // DO NOT DELETE
    return;
  }

  node *curr=prev->next;
  pthread_mutex_lock(&(curr->mutex));
  print_node(prev);
  while(curr->next){
    pthread_mutex_unlock(&(prev->mutex));
	  print_node(curr);
    prev=curr;
	  curr=curr->next;
    pthread_mutex_lock(&(curr->mutex));
  }
  print_node(curr);
  pthread_mutex_unlock(&(prev->mutex));
  pthread_mutex_unlock(&(curr->mutex));
}
printf("\n"); // DO NOT DELETE
return;
}





void count_list(list* list, int (*predicate)(int))
{
  int count = 0; // DO NOT DELETE
  
  if(!list){
    return;
  }

  node *prev = list->head;
  if(prev){
    pthread_mutex_lock(&(prev->mutex));
    if(predicate(prev->value)==1){
      count++;
    }
  }else{
    printf("%d items were counted\n", count); // DO NOT DELETE
    return;
  }

  if(!prev->next){
    pthread_mutex_unlock(&(prev->mutex));
    printf("%d items were counted\n", count);
    return;
  }
  node *curr=prev->next;
  pthread_mutex_lock(&(curr->mutex));

  while(curr->next){
    if(predicate(curr->value)==1){count++;}
    pthread_mutex_unlock(&(prev->mutex));
    prev=curr;
    curr=curr->next;
    pthread_mutex_lock(&(curr->mutex));
  }

  if(predicate(curr->value)==1){
    count++;
  }

  pthread_mutex_unlock(&(curr->mutex));
  pthread_mutex_unlock(&(prev->mutex));

  printf("%d items were counted\n", count); // DO NOT DELETE
  return;
}
