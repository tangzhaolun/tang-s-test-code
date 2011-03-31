#include <unistd.h>
#include <stdio.h>
//#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

int main()
{
    int fd;
    const char *p = "test.txt";
    FILE *fp;
    fd = open (p, O_WRONLY | O_CREAT);
    // judging return value of open, fopen or fdopen is very important,
    // if no judgement here, following file operation may 
    // cause segmentation fault when file fails to open
    if (fd == -1){ 
        printf("open error.\n");
        return -1;
    }

    char str[] = "First string from fdopen-test.";
    // strlen(str) + 1== sizeof(str); 
    // string example about null terminator '\0'
    //char test1[3] = "Hi"; //ok, default: test1[2] = 0
    //char test2[3] = "Hii"; //test2 may point to whatever char until it mets 0
    //printf ("length of %s: %d\n", test1, strlen(test1)); //print 3
    //printf ("length of %s: %d\n", test2, strlen(test2)); //print larger than3
    write (fd, str, sizeof(str)-1);

    fp = fdopen (fd, "w");
    if (fp == NULL){
        printf("fdopen error.\n");
        return -1;
    }

    char str2[] = " second string from fdopen-test.";
    fwrite (str2, sizeof(str2), 1, fp); //sizeof(str2) = strlen(str2)+1
    //null terminator char will be written to fp, which is useless to text 
    fwrite ("\n", 1, 1, fp);

    printf("fclose returns %d.\n", fclose (fp));

    //if fdopen is called after open, it returns new file pointer and
    //closes original fd, so no need to call close afterwards
    printf("close returns %d.\n", close (fd));

    //If call close second time, it returns -1. 
    //However, it seems ok if you call close twice.
    //printf("close returns %d.\n", close (fd));

    return 0;    
}
