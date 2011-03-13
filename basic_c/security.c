/* vim:set et sts=4: */

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>    

int main(int argc, char* argv[])
{
	int uid = geteuid();
	int gid = getegid();
	printf ("uid = %d, gid = %d\n", uid, gid);

	const char* filename = argv[1];
	struct stat buf;

        stat(filename, &buf);
        if(buf.st_mode & S_IWUSR)
            printf("user can write %s\n", filename);
        if(buf.st_mode & S_IRUSR)        
            printf("user can read %s\n'", filename);

	return 0;
}

