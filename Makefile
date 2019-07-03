#------------------------------------------------------------------------------
# Makefile
# Tyler Hoang
# tydhoang
# PA4
# Makefile with macros for DigraphProperties.c that includes target check in order to
# check for memory leaks
#------------------------------------------------------------------------------

FLAGS   = -std=c99 -Wall
SOURCES = Digraph.c Digraph.h DigraphProperties.c List.c List.h
OBJECTS = Digraph.o DigraphProperties.o List.o
EXEBIN  = DigraphProperties
INFILE = DigraphProperties.c

all: $(EXEBIN)

$(EXEBIN) : $(OBJECTS)
	gcc -o $(EXEBIN) $(OBJECTS)

$(OBJECTS) : $(SOURCES)
	gcc -c $(FLAGS) $(SOURCES)

clean :
	rm -f $(EXEBIN) $(OBJECTS)

check :
	valgrind --leak-check=full $(EXEBIN) $(INFILE) outfile
