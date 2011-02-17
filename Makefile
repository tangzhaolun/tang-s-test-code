CFLAGS = -O0 -g
CXXFLAGS = -O0 -g
LDFLAGS = -g 

TARGETS = \
	test_strcpy_w \
	test_strlen_w \
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

clean:
	$(RM) *.o $(TARGETS)
