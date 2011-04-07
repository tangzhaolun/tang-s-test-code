#include <stdio.h>

struct simple1 {
    char c;
    //int i;  //char is extended to 4 bytes in alignment with 4-byte int
    //long long l;  //char is extened to 4 bytes even long long is 8 bytes
    short s;  //char is extended to 2 bytes in alignment with 2-byte short
};

struct simple2 {
    char c;
    //int i;
    //long long l;
    short s;
}__attribute__((packed));

//if there is only char, every char is 1 byte in memory alignment
struct mychar1 {
    char c1;
    char c2;
    char c3;
    //short s;
};

struct mychar2 {
    char c1;
    char c2;
    char c3;
    //short s;
}__attribute__((packed));

/* aligned attribute expands allocated boundary, is contrary to packed*/
struct aligned {
    int x;
    short y;
}__attribute__((aligned(16)));
/* sizeof aligned should be 8 without attribute,
 * and be 6 with packed attribute,
 * and be 16 with aligned attribute*/

int main() {
    struct simple1 s1;
    struct simple2 s2;
    printf("sizeof s1: %d\n", sizeof(s1));
    printf("sizeof s2: %d\n", sizeof(s2));

    struct mychar1 my1;
    struct mychar2 my2;
    printf("sizeof mychar1: %d\n", sizeof(my1));
    printf("sizeof mychar2: %d\n", sizeof(my2));

    struct aligned al;
    printf("aligned int: %d\n", sizeof(al));

}
