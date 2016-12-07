
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main() {
    char* memory = malloc(3000000000);
    int pid = getpid();
    printf("%d\n", pid);
    sleep(120);
    free(memory);
    return 0;
}
