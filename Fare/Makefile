# Универсальный Makefile

# Компилятор и флаги
CC = gcc
CFLAGS = -std=gnu17 -Wall -Wextra -MMD

# Через пробел имена всех исполняемых файлов
# Если их > 1
PROGRAMS = fare_cards

# Укажи с новой строки через пробел, какие исходники входят в каждую программу
fare_cards_SRCS = fare_cards.c queue.c calculations.c file.c

# Общие правила
OBJS = $(foreach prog,$(PROGRAMS),$($(prog)_SRCS:.c=.o))
DEPS = $(OBJS:.o=.d)

# Правило по умолчанию - собрать все программы
all: $(PROGRAMS)

# Правило сборки для каждой программы
$(PROGRAMS):
	@echo "==> Собирается $@"
	$(CC) $(CFLAGS) -o $@ $($@_SRCS)

# Автоматическое подключение зависимостей
-include $(DEPS)

# Очистка временых файлов и бинарников
clean:
	rm -f $(OBJS) $(DEPS) $(PROGRAMS)

# Красивый вывод
.PHONY: all clean
