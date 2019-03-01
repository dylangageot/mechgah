# -----------------------------------------------------------------------------
# INSA NES Emulator
# -----------------------------------------------------------------------------

# target definition
OUTNAME		= main
UTEST		= unit_test

# directories and sources definition
NESDIR		= src/nes
SRC  		= $(NESDIR)/mapper/nrom.c

# use gcc
CC			= gcc
# compilation options
CFLAGS  	= -Wall -Wextra -MMD
# linking options
LDFLAGS 	= -lcmocka 

# add debug option to gcc if needed
DEBUG = no
ifeq ($(DEBUG),yes)
	CFLAGS += -g
endif

# compile individual object files
OBJS    	= $(SRC:.c=.o)
%.o: %.c
			  $(CC) $(CFLAGS) -c $< -o $@

all: $(OUTNAME) $(UTEST)

# executable compilation
$(OUTNAME): $(OUTNAME).o $(OBJS) $(SRC)
			  $(CC) $< $(OBJS) $(LDFLAGS) -o $@

# unit test executable compilation 
$(UTEST): $(UTEST).o $(OBJS) $(SRC)
			  $(CC) $< $(OBJS) $(LDFLAGS) -o $@

# run unit test and generate coverage page
run-test: CFLAGS  += -coverage
run-test: LDFLAGS += -coverage
run-test: $(UTEST)
		valgrind ./$(UTEST) ; \
		lcov --capture --directory . --output-file coverage.info ; \
		genhtml coverage.info --output-directory out

# cleaning rule
clean:
	rm -f *.o *.d $(OBJS) $(SRC:.c=.gcda) $(SRC:.c=.d) $(OUTNAME) $(UTEST) \
	$(SRC:.c=.gcno) $(SRC:.c=.gcov) *.gcda *.gcno *.gcov *.info *~ -r out

include $(shell find -name '*.d' | sed 's\./\\')  
