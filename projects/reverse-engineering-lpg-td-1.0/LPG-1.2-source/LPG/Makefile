#####################
# Makefile for LPG  #
#####################


####### FLAGS

CFLAGS 	= -g -DNDEBUG


LIBS    = -lm  


TARGET = lpg
CFLAGS = -O3 -Wall
LIBS=  -lm 


####### COMPILE

CC	= gcc 

PATH_HEADERS	= include


SOURCES = main.c \

OBJECTS = $(SOURCES:.c=.o) 

LPG_LIBS = LocalSearch/LocalSearch.a Utilities/Utilities.a HillClimbingSearch/HillClimbingSearch.a Parser/Parser.a

SUB_DIRS = LocalSearch Utilities HillClimbingSearch Parser

find_header = $(wildcard $(dir)/*.h)

HEADERS := $(foreach dir,./include,$(find_header))

####### Implicit rules

.SUFFIXES:

.SUFFIXES: .c .o

.c.o:; $(CC) -I$(PATH_HEADERS) -c $(CFLAGS) $<


####### Build rules

lpg:	$(OBJECTS) COMPILE_SUB
	$(CC) -o $(TARGET) $(OBJECTS) $(LPG_LIBS) $(CFLAGS) $(LIBS)

COMPILE_SUB:
	@ for i in $(SUB_DIRS); do $(MAKE) -C $$i; done

$(OBJECTS): $(HEADERS)

clean:
	@ for i in $(SUB_DIRS); do $(MAKE) -C $$i clean; done
	-rm -f *.o *~ $(PATH_HEADERS)/*~

distclean:
	@ for i in $(SUB_DIRS); do $(MAKE) -C $$i clean; done
	-rm -f *.o *~ lpg* $(PATH_HEADERS)/*~
#######
