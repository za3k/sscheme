CFLAGS = -Wall -Werror -std=gnu99 -g -O0 -fdiagnostics-color=always
C_FILES = $(wildcard *.c)
OFILES = $(subst %.c,%.o,$(C_FILES))

all: eval

clean:
	rm -f eval *.o
#eval.o: test.c *.h $(OFILES)
eval: eval.o
	$(CC) $(CFLAGS) -o $@ $(OFILES)
