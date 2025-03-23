#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[]){
    fork();
    printf("Hello word\n");
    getc(stdin);
    return 0;
}