CFLAGS = -Wall -Werror -std=gnu99 -g -O0 -fdiagnostics-color=always -fdump-rtl-expand 
C_FILES = $(wildcard *.c)
DATA_FILES = $(wildcard *.txt)
DATA_H_FILES = $(patsubst %.txt,%.inc.h,$(DATA_FILES))
H_FILES = $(DATA_H_FILES) $(wildcard *.h)
O_FILES = $(patsubst %.c,%.o,$(C_FILES))

all: eval callgraph

clean:
	rm -f eval *.o *.inc.h *.expand callgraph.png
callgraph: eval
	egypt *.expand --callees eval_all --omit isstring,issymbol,iserror,isempty,ispair,error,make_int,pred,make_cell,make_int,isnumber,symboleq,ischar,make_cons,cdr,car,cons,islistoflength,isenv,ismacro,make_macro,isfalse,append,reverse,iseqv | sed "s/_eval/eval/;s/_apply/apply/" | dot -Tpng -o callgraph.png
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
