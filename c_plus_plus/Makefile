CFLAGS = -O0 -g -Wall
CXXFLAGS = $(CFLAGS)
LDFLAGS = -g -lpthread

# The TARGETS defines all the final executable files.
# DON'T write .c files in TARGETS!! They will be cleaned by make clean.
TARGETS = \
	tcpclient \
	tcpserver \
	$(NULL)

# if 'all' is not defined, make command will execute
# only the first dependency line,
# so only one executable file from the first line will be created.
all: $(TARGETS)


# NOTE: when you write the dependency from .o to executable file,
#	you MUST start the second line with a TAB, not blackspaces. 

tcpclient: ctcpsocket.o testtcpclient.o
	$(CXX) $(LDFLAGS) $^ -o $@

tcpserver: ctcpsocket.o testtcpserver.o
	$(CXX) $(LDFLAGS) $^ -o $@

clean:
	$(RM) *.o *~ a.out $(TARGETS)
