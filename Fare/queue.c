#include <stdlib.h>

#include "queue.h"

// инициализация и обнуление очереди
void init_queue(Queue *q)
{
	if(!q) return;
	q->head = q->tail = NULL;
	q->cnt = 0;
}

// добавление элементов в очередь
bool enqueue(Queue *q, Data d)
{
	if(!q) return false;
	
	Node *new_node = malloc(sizeof(*new_node));
	if(!new_node){
		return false;
	}
	new_node->data = d;
	new_node ->next = NULL;
	
	if(!q->head){
		q->head = q->tail = new_node;
	} else{
		q->tail->next = new_node;
		q->tail = new_node;
	}
	q->cnt++;
	return true;
}

// освобождаем память очереди
void free_queue(Queue *q)
{
	if(!q) return;
	
	Node *cur = q->head;
	while(cur){
		Node *next = cur->next;
		free(cur);
		cur = next;
	}
	q->head = q->tail = NULL;
	q->cnt = 0;
}