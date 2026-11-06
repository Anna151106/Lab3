#include <stdio.h>

int a[10];
int n = 10;

int print_array() {
    int i;
    for (i = 0; i < n; i++)
        printf("%d ", a[i]);
    printf("\n");
    return 0;
}

int bubble_sort() {
    int i;
    int j;
    int tmp;
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - 1 - i; j++) {
            if (a[j] > a[j + 1]) {
                tmp = a[j];
                a[j] = a[j + 1];
                a[j + 1] = tmp;
            }
        }
    }
    return 0;
}

int main() {
    a[0] = 42;
    a[1] = 7;
    a[2] = -3;
    a[3] = 19;
    a[4] = 0;
    a[5] = 88;
    a[6] = 7;
    a[7] = 15;
    a[8] = -10;
    a[9] = 3;
    printf("before: ");
    print_array();
    bubble_sort();
    printf("after:  ");
    print_array();
    return 0;
}
