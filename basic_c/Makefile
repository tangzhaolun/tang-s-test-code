CFLAGS = -O0 -g -Wall
CXXFLAGS = $(CFLAGS)
LDFLAGS = -g -lpthread

# The TARGETS defines all the final executable files.
# DON'T write .c files in TARGETS!! They will be cleaned by make clean.
TARGETS = \
	gnu-attribute \
	swap-64bit \
	$(NULL)

# if 'all' is not defined, make command will execute
# only the first dependency line,
# so only one executable file from the first line will be created.
all: $(TARGETS)

# the following two lines are included in make as default,
# so you can remove them and all .c files will still be compiled to .o files.
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

# NOTE: when you write the dependency from .o to executable file,
#	you MUST start the second line with a TAB, not blackspaces. 


gnu-attribute: gnu-attribute.o
	$(CC) $(LDFLAGS) $^ -o $@

swap-64bit: swap_64bit.o
	$(CC) $(LDFLAGS) $^ -o $@

clean:
	$(RM) *.o *~ a.out $(TARGETS)
