#ifndef TYPES_H
#define TYPES_H

#ifndef BUF_SIZE
	#define BUF_SIZE 1024
#endif

#define FIRST_DAY 1

enum {
	PRICE_BANK_CARD = 20,
	RETURN_PRICE_TICKET = 28,
	PRICE_TICKET = 35,
	PRICE_FUEL = 160
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