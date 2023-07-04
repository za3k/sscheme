CFLAGS = -Wall -Werror -std=gnu99 -g -O0 -fdiagnostics-color=always
C_FILES = $(wildcard *.c)
DATA_FILES = $(wildcard *.txt)
DATA_H_FILES = $(patsubst %.txt,%.inc.h,$(DATA_FILES))
H_FILES = $(DATA_H_FILES) $(wildcard *.h)
O_FILES = $(patsubst %.c,%.o,$(C_FILES))

all: eval

clean:
	rm -f eval *.o *.inc.h *.expand callgraph.png
$(O_FILES): $(H_FILES)
%.inc.h: %.txt
	xxd -i $< | sed -e "s/};/,0x00};/" | sed -e "s/unsigned //" >$@
eval: main.o $(O_FILES)
	$(CC) $(CFLAGS) -o $@ $(O_FILES)
eval-static: main.o $(O_FILES)
	musl-gcc -static $(CFLAGS) -o $@ $(O_FILES)
	strip $@
test: eval
	./eval --test || gdb -ex 'set args --test' -ex 'run' ./eval
