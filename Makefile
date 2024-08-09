CC     :=  gcc
CFLAGS :=  -std=c99 -W -Wall -Wextra -Wpedantic

SRCS := term.c parser.c reduction.c LC.c
OBJS := $(addprefix obj/,$(SRCS:.c=.o))
SRCS := $(addprefix src/,$(SRCS))

all: LC

LC: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

obj/%.o: src/%.c | obj
	$(CC) $(CFLAGS) -o $@ -c $<

obj:
	mkdir -p obj

.PHONY: all
