#include <stdio.h>
#include "params.h"
#include "polyvec.h"

void print_matrix(polyvec *A) {
    printf("Matriz A generada:\n");
    for (int i = 0; i < KYBER_K; i++) {
        for (int j = 0; j < KYBER_K; j++) {
            printf("A[%d][%d]: ", i, j);
            for (int k = 0; k < 4; k++) {
                printf("%d ", A[i].vec[j].coeffs[k]);
            }
            printf("\n");
        }
    }
}

