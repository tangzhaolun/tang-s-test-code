CFLAGS = -O0 -g -Wall
CXXFLAGS = $(CFLAGS)
LDFLAGS = -g 

TARGETS = \
	test_strcpy_w \
	test_strlen_w \
	char_to_asci \
	parameter_in_stack_w\
	$(NULL)

all: $(TARGETS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

test_strcpy_w: strcpy_w.o
	$(CC) $(LDFLAGS) $^ -o $@

test_strlen_w: strlen_w.o
	$(CC) $(LDFLAGS) $^ -o $@

char_to_asci: char_to_asci.o
	$(CC) $(LDFLAGS) $^ -o $@

parameter_in_stack_w: parameter_in_stack_w.o
	$(CC) $(LDFLAGS) $^ -o $@

clean:
	$(RM) *.o $(TARGETS)
