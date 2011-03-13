CFLAGS = -O0 -g -Wall
CXXFLAGS = $(CFLAGS)
LDFLAGS = -g -lpthread

# The TARGETS defines all the final executable files.
# DON'T write .c files in TARGETS!! They will be cleaned by make clean.
TARGETS = \
	thread_job_queue \
	thread_job_queue_sem \
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

thread_job_queue: thread_job_queue.o
	$(CC) $(LDFLAGS) $^ -o $@

thread_job_queue_sem: thread_job_queue_sem.o
	$(CC) $(LDFLAGS) $^ -o $@


clean:
	$(RM) *.o *~ a.out $(TARGETS)
