CFLAGS = -Wall -Werror -std=gnu99 -g -O0 -fdiagnostics-color=always
C_FILES = $(wildcard *.c)
H_FILES = $(wildcard *.h)
O_FILES = $(subst %.c,%.o,$(C_FILES))

all: eval

clean:
	rm -f eval *.o
$(OFILES): $(H_FILES)
eval: main.o $(O_FILES)
	$(CC) $(CFLAGS) -o $@ $(O_FILES)
