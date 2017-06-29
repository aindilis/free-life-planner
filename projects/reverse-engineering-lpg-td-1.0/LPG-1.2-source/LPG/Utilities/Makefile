#####################
# Makefile for LPG  #
#####################


####### FLAGS

CFLAGS = -g -DNDEBUG


TARGET = lpg
CFLAGS = -O3 -DNDEBUG  -Wall
LIBS=  -lm 


####### COMPILE

CC	= gcc 

PATH_HEADERS = -I$(SRC_HEADER)

SRC_HEADER =../include

SOURCES = utilities.c \
	LpgOutput.c \
	check.c \
	mutex.c

OBJECTS = $(SOURCES:.c=.o)

find_header = $(wildcard $(dir)/*.h)

HEADERS := $(foreach dir,../include,$(find_header))


####### Implicit rules

.SUFFIXES:

.SUFFIXES: .c .o

.c.o:; $(CC) $(PATH_HEADERS) -c $(CFLAGS) $<


####### Build rules

Utilities.a: 	$(OBJECTS) 
		$(AR) rc $@ $^

$(OBJECTS): $(HEADERS)

clean: 	
	rm -f *.o *~ *.a

#######
