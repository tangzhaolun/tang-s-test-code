CFLAGS = -O0 -g -Wall
CXXFLAGS = $(CFLAGS)
LDFLAGS = -g 

# The TARGETS defines all the final executable files.
TARGETS = \
	test_strcpy_w \
	test_strlen_w \
	char_to_asci \
	parameter_in_stack_w\
	security\
	$(NULL)

# if 'all' is not defined, make command will only execute the first dependency line from .o to executable file.
# for example, if 'all' is removed, only test_strcpy_w will be produced.
all: $(TARGETS)

# the following two lines are included in make as default,
# so you can remove them and all .c files will still be compiled to .o files.
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

# define the dependency line from .o to executable file
test_strcpy_w: strcpy_w.o
	$(CC) $(LDFLAGS) $^ -o $@

test_strlen_w: strlen_w.o
	$(CC) $(LDFLAGS) $^ -o $@

char_to_asci: char_to_asci.o
	$(CC) $(LDFLAGS) $^ -o $@

parameter_in_stack_w: parameter_in_stack_w.o
	$(CC) $(LDFLAGS) $^ -o $@

security: security.o
	$(CC) $(LDFLAGS) $^ -o $@

clean:
	$(RM) *.o $(TARGETS)
