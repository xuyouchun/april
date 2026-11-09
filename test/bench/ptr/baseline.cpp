#include <cstdio>
#include <cstdint>
#include <cstdlib>
int main()
{
    int n = 4096;
    int * a = (int *)malloc(n * 4);
    for (int i = 0; i < n; i++) a[i] = i;
    int64_t total = 0;
    for (int r = 0; r < 50000; r++)
    {
        int s = 0;
        int * p = a;
        for (int i = 0; i < n; i++) { s += *p; p++; }
        total += s;
    }
    printf("%lld\n", (long long)total);
    free(a);
}
