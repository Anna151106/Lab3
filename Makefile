BISON = $(shell if [ -x /opt/homebrew/opt/bison/bin/bison ]; then echo /opt/homebrew/opt/bison/bin/bison; else echo bison; fi)
FLEX = flex
CC = gcc
CFLAGS = -Wall -Isrc -Ibuild

OBJS = build/parser.o build/lexer.o build/ast.o build/interp.o build/pygen.o build/json.o build/main.o

.PHONY: all web clean

all: cparser

cparser: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

build/parser.c build/parser.h: src/parser.y
	mkdir -p build
	$(BISON) -d -o build/parser.c src/parser.y

build/lexer.c: src/lexer.l build/parser.h
	$(FLEX) -o build/lexer.c src/lexer.l

build/parser.o: build/parser.c src/ast.h
	$(CC) $(CFLAGS) -c build/parser.c -o $@

build/lexer.o: build/lexer.c src/ast.h
	$(CC) $(CFLAGS) -c build/lexer.c -o $@

build/%.o: src/%.c src/ast.h build/parser.h
	$(CC) $(CFLAGS) -c $< -o $@

web: cparser
	echo "var programs = [" > web/data.js
	for f in examples/*.c; do ./cparser $$f --json >> web/data.js || exit 1; echo "," >> web/data.js; done
	echo "];" >> web/data.js

clean:
	rm -rf build cparser out.py
