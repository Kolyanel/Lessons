#ifndef CALCULATIONS_H
#define CALCULATIONS_H

#include "types.h"

// считывает ввод пользователя и формирует данные за один день
Data create_data(Period d);

// вывод общего результата
void print_res(Calc res);

#endif // CALCULATIONS_H