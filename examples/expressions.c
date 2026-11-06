#include <stdio.h>

int counter = 0;

int next_id() {
    counter = counter + 1;
    return counter;
}

int gcd(int a, int b) {
    if (b == 0)
        return a;
    return gcd(b, a % b);
}

int sign(int x) {
    if (x >= 0)
        if (x == 0)
            return 0;
        else
            return 1;
    return -1;
}

int main() {
    int a = 2 + 3 * 4 - 10 / 3;
    int b = (2 + 3) * (4 - 1);
    int c = 100 - 20 - 30;
    printf("a=%d b=%d c=%d\n", a, b, c);

    printf("-7/2=%d -7%%2=%d 7/-2=%d 7%%-2=%d\n", -7 / 2, -7 % 2, 7 / -2, 7 % -2);

    int l = (a > b) + (b > c) * 10 + (3 && 5) * 100 + (0 || 0) * 1000;
    printf("logic=%d !5=%d !0=%d\n", l, !5, !0);
    printf("a<=b: %d, a!=b: %d, a==a: %d, -a+b: %d\n", a <= b, a != b, a == a, -a + b);

    printf("sign: %d %d %d\n", sign(-5), sign(0), sign(8));
    printf("gcd(84, 36) = %d\n", gcd(84, 36));

    int id1 = next_id();
    int id2 = next_id();
    printf("ids: %d %d, counter=%d\n", id1, id2, counter);

    int k = 10;
    int s = 0;
    while (k > 0) {
        s = s + k;
        k--;
    }
    printf("sum 1..10 = %d\n", s);
    return 0;
}
