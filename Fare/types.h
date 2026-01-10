#ifndef TYPES_H
#define TYPES_H

#define BUF_SIZE 1024
#define FIRST_DAY 1

enum {
	PRICE_BANK_CARD = 17,
	PRICE_TICKET = 24,
	PRICE_EGCS = 30,
	PRICE_FUEL = 75
	};
	
typedef enum{
	ERR,
	WRITE,
	APPEND
} Indicator;

typedef struct {
	int day;
	int month;
} Period;

typedef struct {
	Period date;
	float socCard;
	int bankCard;
	int egcsCard;
} Data;

typedef struct{
	float fuel_total;
	int bank_total;
	int egcs_total;
} Calc;

#endif  //TYPES_H