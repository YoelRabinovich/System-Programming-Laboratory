assembler:	assembler.o	table.o	line_parser.o	coder.o	string.o	files.o	passes.o
	gcc	-g	assembler.o	passes.o	table.o	line_parser.o	coder.o	string.o	files.o	-pedantic	-Wall	-o	assembler
assembler.o:	assembler.c	assembler.h	line_parser.h	coder.h	passes.h table.h	files.h
	gcc	-c	assembler.c	-ansi	-pedantic	-Wall	-o	assembler.o
passes.o:	passes.c passes.h	assembler.h	string.h	coder.h	table.h	line_parser.h
	gcc	-c	passes.c -ansi	-pedantic	-Wall	-o	passes.o
table.o:	table.c	table.h	coder.h	files.h
	gcc	-c	table.c	-ansi	-pedantic	-Wall	-o	table.o
line_parser.o:	line_parser.c	line_parser.h	assembler.h	string.h	passes.h
	gcc	-c	line_parser.c	-ansi	-pedantic	-Wall	-o	line_parser.o
coder.o:	coder.c	coder.h	assembler.h	files.h	table.h
	gcc	-c	coder.c	-ansi	-pedantic	-Wall	-o	coder.o
string.o:	string.c	string.h
	gcc	-c	string.c	-ansi	-pedantic	-Wall	-o	string.o
files.o:	files.c	files.h
	gcc	-c	files.c	-ansi	-pedantic	-Wall	-o	files.o
