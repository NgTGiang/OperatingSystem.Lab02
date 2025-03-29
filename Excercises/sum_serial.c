#include <stdio.h>
#include <stdlib.h>

long long sum(int n) {
    long long total = 0;
    for (int i = 1; i <= n; i++) {
        total += i;
    }
    return total;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <n>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);
    long long result = sum(n);
    printf("Sum of 1 to %d is %lld\n", n, result);

    return EXIT_SUCCESS;
}
