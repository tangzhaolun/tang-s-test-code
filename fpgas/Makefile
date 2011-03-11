CFLAGS = -O0 -g
CDFLAGS = $(CFLAGS)

TARGETS = \
	udpreceiver \
	testsender \
	$(NULL)
   	 
ALL: $(TARGETS)

udpreceiver: udpreceiver.o
	$(CC) $(CFLAG) $^ -o $@
    
testsender: testsender.o
	$(CC) $(CFLAG) $^ -o $@
    
clean:
	$(RM) *.o $(TARGETS)

