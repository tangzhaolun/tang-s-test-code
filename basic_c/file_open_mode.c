#include <unistd.h>
#include <stdio.h>
//#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

int main()
{
    const char *p = "test.txt";
    char buf[100];
    int len;

    FILE *fp;

    //recommend to memset buffer contents before using it
    memset (buf, 0, sizeof buf);
    strcpy (buf, "\tHello.\t");

    //w+, r+, a+ allow reading and writing 
    //fp = fopen (p, "r+"); //insert at the start of file
    //fp = fopen (p, "a+"); //append at the end of file
    //fp = fopen (p, "w+"); //erase cotent if file exists,always at start
    
    //fp = fopen (p, "w"); //only for writing, erase content if file exists
    fp = fopen (p, "a"); //only for writing (appending at end)
    
    //fp = fopen (p, "r"); //only for reading, fwrite fail in this mode

    //important to judge the return value of fopen, or open, fdopen
    //otherwise file operation on open-failed file cause segmentation error
    if (fp == NULL){
        printf ("fopen error\n");
        return -1;
    }

    //both following fwrite are ok, write same bytes to fp
    printf("bytes written: %d\n", fwrite (buf, 1, sizeof(buf), fp));
    printf("bytes written: %d\n", fwrite (buf, sizeof(buf), 1, fp));
    
    //put end-of-line char to file end, to make reading easier    
    fputc('\n', fp);
    printf("fclose returns %d.\n", fclose (fp));

    return 0;    
}
