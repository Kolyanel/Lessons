#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <stddef.h>
#include "types.h"

typedef struct node {
	Data data;
	struct node *next;
} Node;

typedef struct {
	Node *head; //начало очереди
	Node *tail; // конец очереди
	size_t cnt; // количество элементов в очереди
}  Queue;


// инициализация и обнуление очереди
void init_queue(Queue *q);

// добавление элементов в очередь
bool enqueue(Queue *q, Data d);

// освобождаем память очереди
void free_queue(Queue *q);

#endif //QUEUE_H