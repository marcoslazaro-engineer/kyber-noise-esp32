#include <stdio.h>
#include <stdint.h>
#include "esp_random.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "params.h"
#include "indcpa.h"
#include "polyvec.h"
#include "kyber_utils.h"

void app_main(void) {
    static polyvec A[KYBER_K];
    uint8_t seed[32];

    esp_fill_random(seed, 32);

    printf("Seed usada:\n");
    for (int i = 0; i < 32; i++) {
        printf("%02X", seed[i]);
    }
    printf("\n\n");

    gen_matrix(A, seed, 0);
    print_matrix(A);

    while (1) {
        vTaskDelay(portMAX_DELAY);
    }
}

