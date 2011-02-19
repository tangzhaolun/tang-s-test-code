CFLAGS = -O3 -g
CXXFLAGS = -O0 -g
LDFLAGS = -g 

TARGETS = \
	test_strcpy_w \
	test_strlen_w \
	char_to_asci \
	$(NULL)


all: $(TARGETS)


strcpy_w.o: strcpy_w.c
	$(CC) $(CFLAGS) -c $< -o $@

test_strcpy_w: strcpy_w.o
	$(CC) $(LDFLAGS) $^ -o $@

strlen_w.o: strlen_w.c
	$(CC) $(CFLAGS) -c $< -o $@

test_strlen_w: strlen_w.o
	$(CC) $(LDFLAGS) $^ -o $@


char_to_asci.o: char_to_asci.c
	$(CC) $(CFLAGS) -c $< -o $@

char_to_asci: char_to_asci.o
	$(CC) $(LDFLAGS) $^ -o $@

clean:
	$(RM) *.o $(TARGETS)
