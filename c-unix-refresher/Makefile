# Define variable CC to be the compiler we want to use
CC = clang

# Define CFLAGS for the flags we will want to use with clang
CFLAGS = -g -Wall

TARGETS = clean c-example

# If no arguments are passed to make, it will attempt the 'c-example' target
default: c-example

# This runs the 'clean' and 'c-example' targets when 'make all' is run
all: $(TARGETS)

# This will construct the binary 'c-example'
# $^ = names of all the dependent files, deduped and with spaces
# $@ = complete name of the target
c-example: c-example.c
	$(CC) $(CFLAGS) $< -o $@

# $(RM) is the platform agnostic way to delete a file (here rm -f)
clean:
	$(RM) -rf c-example *~ *dSYM
