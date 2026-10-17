/* STREAM benchmark */
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 16000000

static double *a, *b, *c;

int main(void)
{
    int i;
    double scalar = 3.0;
    double t0, t1;
    double times[4];
    const char *names[4] = {"Copy", "Scale", "Add", "Triad"};

    a = (double*)malloc(N * sizeof(double));
    b = (double*)malloc(N * sizeof(double));
    c = (double*)malloc(N * sizeof(double));

    #pragma omp parallel
    {
        #pragma omp for
        for(i = 0; i < N; i++){
            a[i] = 1.0;
            b[i] = 2.0;
            c[i] = 0.0;
        }
    }

    // Copy: c = a
    t0 = omp_get_wtime();
    #pragma omp parallel for
    for(i = 0; i < N; i++) c[i] = a[i];
    t1 = omp_get_wtime();
    times[0] = t1 - t0;

    // Scale: b = scalar*c
    t0 = omp_get_wtime();
    #pragma omp parallel for
    for(i = 0; i < N; i++) b[i] = scalar * c[i];
    t1 = omp_get_wtime();
    times[1] = t1 - t0;

    // Add: c = a + b
    t0 = omp_get_wtime();
    #pragma omp parallel for
    for(i = 0; i < N; i++) c[i] = a[i] + b[i];
    t1 = omp_get_wtime();
    times[2] = t1 - t0;

    // Triad: a = b + scalar*c
    t0 = omp_get_wtime();
    #pragma omp parallel for
    for(i = 0; i < N; i++) a[i] = b[i] + scalar * c[i];
    t1 = omp_get_wtime();
    times[3] = t1 - t0;

    double bytes[4] = {
        2.0 * N * sizeof(double),
        2.0 * N * sizeof(double),
        3.0 * N * sizeof(double),
        3.0 * N * sizeof(double),
    };

    printf("STREAM version C\n");
    printf("Array size = %d, Offset = 0\n", N);
    printf("\n%7s %15s %15s\n", "Kernel", "MB/s", "Time(s)");
    for(i=0;i<4;i++){
        double bw = bytes[i] / (1024.0*1024.0) / times[i];
        printf("%7s %15.2f %15.4f\n", names[i], bw, times[i]);
    }

    free(a);free(b);free(c);
    return 0;
}
