#####################
# Makefile for LPG  #
#####################


####### FLAGS

CFLAGS = -O3 -DNDEBUG


TARGET = lpg
CFLAGS = -O3 -DNDEBUG  -Wall
LIBS=  -lm 


####### COMPILE

CC	= gcc 

PATH_HEADERS = -I$(SRC_HEADER)

SRC_HEADER =../include


####### Files

PDDL_PARSER_SRC	= scan-fct_pddl.tab.c \
	scan-ops_pddl.tab.c \
	scan-probname.tab.c \
	lex.fct_pddl.c \
	lex.ops_pddl.c 

PDDL_PARSER_OBJ = scan-fct_pddl.tab.o \
	scan-ops_pddl.tab.o 

SOURCES = memory.c \
	parse.c \
	inst_pre.c \
	inst_easy.c \
	inst_hard.c \
	inst_final.c \
	inst_utils.c

HEADERS = inst_easy.h \
	inst_hard.h\
	memory.h  \
	ff.h   \
	inst_final.h \
	inst_pre.h  \
	parse.h

OBJECTS 	= $(SOURCES:.c=.o)

find_header = $(wildcard $(dir)/*.h)

HEADERS := $(foreach dir,../include,$(find_header))


####### Implicit rules

.SUFFIXES:

.SUFFIXES: .c .o

.c.o:
	 $(CC) $(PATH_HEADERS) -c $(CFLAGS) $<


####### Build rules

Parser.a: 	$(OBJECTS) $(PDDL_PARSER_OBJ)
		$(AR) rc $@ $^

$(OBJECTS): $(HEADERS)

scan-fct_pddl.tab.c: scan-fct_pddl.y lex.fct_pddl.c
	bison --verbose -pfct_pddl -bscan-fct_pddl scan-fct_pddl.y

scan-ops_pddl.tab.c: scan-ops_pddl.y lex.ops_pddl.c
	bison --verbose -pops_pddl -bscan-ops_pddl scan-ops_pddl.y

lex.fct_pddl.c: lex-fct_pddl.l
	flex -Pfct_pddl lex-fct_pddl.l

lex.ops_pddl.c: lex-ops_pddl.l
	flex -Pops_pddl lex-ops_pddl.l

####### clean

clean:
	rm -f *.o *.output *~ *.a $(PDDL_PARSER_SRC)

#######
