# -----------------------------------------------------------------------------
# INSA NES Emulator
# -----------------------------------------------------------------------------

# target definition
OUTNAME		= mechgah
UTEST		= utest

# directories and sources definition
SRCDIR		= src
NESDIR		= $(SRCDIR)/nes
UTESTDIR	= $(SRCDIR)/unit-test
COMMONDIR	= $(SRCDIR)/common
SRC  		= $(NESDIR)/mapper/nrom.c \
			  $(NESDIR)/mapper/mapper.c \
			  $(NESDIR)/mapper/ioreg.c \
			  $(NESDIR)/loader/loader.c \
			  $(NESDIR)/cpu/instruction.c \
			  $(NESDIR)/cpu/cpu.c \
			  $(NESDIR)/ppu/ppu.c \
			  $(NESDIR)/nes.c \
			  $(NESDIR)/controller/controller.c \
			  $(NESDIR)/controller/joypad.c \
			  $(SRCDIR)/app.c \
			  $(UTESTDIR)/UTnrom.c \
			  $(UTESTDIR)/UTinstruction.c \
			  $(UTESTDIR)/UTloader.c \
			  $(UTESTDIR)/UTcpu.c \
			  $(UTESTDIR)/UTstack.c \
			  $(UTESTDIR)/UTppu.c \
			  $(UTESTDIR)/UTioreg.c \
			  $(UTESTDIR)/UTjoypad.c \
			  $(UTESTDIR)/UTcontroller.c \
			  $(UTESTDIR)/UTkeys.c \
			  $(UTESTDIR)/UTnes.c \
			  $(COMMONDIR)/keys.c \
			  $(COMMONDIR)/stack.c \

# use gcc
CC			= gcc
# compilation options
CFLAGS  	= -Wall -Wextra -MMD
# linking options
LDFLAGS 	= -lcmocka -lSDL -lSDL_gfx

# add debug option to gcc if needed
DEBUG = no
ifeq ($(DEBUG),yes)
	CFLAGS += -g -DDEBUG_CPU
endif

# compile individual object files
OBJS    	= $(SRC:.c=.o)
%.o: %.c
			 $(CC) $(CFLAGS) -c $< -o $@

all: $(OUTNAME)

# executable compilation
$(OUTNAME): main.o $(OBJS) $(SRC)
			  $(CC) $< $(OBJS) $(LDFLAGS) -o $@

# unit test executable compilation
$(UTEST): $(UTESTDIR)/UTest.o $(OBJS) $(SRC)
			  $(CC) $< $(OBJS)  $(LDFLAGS) -o $@

# run unit test and generate coverage page
run-test: CFLAGS  += -coverage -DDEBUG_CPU
run-test: LDFLAGS +=  -Wl,--wrap=SDL_PollEvent -coverage
run-test: $(UTEST)
		valgrind --leak-check=full --show-leak-kinds=all ./$(UTEST) ; \
		lcov --capture --directory . --output-file coverage.info ; \
		genhtml coverage.info --output-directory out

doc:
	doxygen doxyfile

# cleaning rule
clean:
	rm -f *.o *.d $(OBJS) $(SRC:.c=.gcda) $(SRC:.c=.d) $(OUTNAME) $(UTEST) \
	$(SRC:.c=.gcno) $(SRC:.c=.gcov) *.gcda *.gcno *.gcov *.info *~ -r out  \
	*.log doxygen

include $(shell find -name '*.d' | sed 's\./\\')
