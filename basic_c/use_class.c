#include <stdlib.h>
#include <stdio.h>

typedef struct MyStruct{
    int id;
    struct MyStruct *next;
}MyClass;

void initialize (MyClass **c, int i){
    *c = (MyClass *)malloc (sizeof (MyClass));
    (*c)->id = i;
    (*c)->next = NULL;
    printf("Initialize OK: %d\n", (*c)->id);
}

void cleanup (MyClass **c){
    if (*c != NULL) {
        free (*c);
        printf ("free pointer\n");
    }
    printf("Cleanup done.\n");
}

int main(){
    MyClass *c;
    initialize(&c, 10);
    cleanup(&c);
    return 0;
}
