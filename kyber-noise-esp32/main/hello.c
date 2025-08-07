 #include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_random.h"

#include "params.h"
#include "poly.h"
#include "cbd.h"
#include "ntt.h"
#include "randombytes.h"
#include "symmetric.h"


#define TAG "KYBER"

void print_poly(const char *name, poly *p) {
    printf("%s: ", name);
    for (int i = 0; i < KYBER_N; i++) {
        printf("%d ", p->coeffs[i]);
        if ((i + 1) % 16 == 0) printf("\n");
    }
    printf("\n\n");
}

void app_main(void) {
    ESP_LOGI(TAG, "Generating noise polynomials for Kyber512...");

    uint8_t seed[KYBER_SYMBYTES];
    poly s, e, r, e1, e2;

    // Generate a random seed
    randombytes(seed, KYBER_SYMBYTES);

    ESP_LOGI(TAG, "Seed:");
    for (int i = 0; i < KYBER_SYMBYTES; i++) printf("%02X", seed[i]);
    printf("\n\n");

    // Buffers to hold pseudorandom output for CBD
   static uint8_t buf_s[KYBER_ETA1 * KYBER_N / 4];
   static uint8_t buf_e[KYBER_ETA1 * KYBER_N / 4];
   static uint8_t buf_r[KYBER_ETA1 * KYBER_N / 4];
   static uint8_t buf_e1[KYBER_ETA1 * KYBER_N / 4];
   static uint8_t buf_e2[KYBER_ETA1 * KYBER_N / 4];

    // Generate pseudorandom data with nonce for each
    kyber_shake256_prf(buf_s, sizeof(buf_s), seed, 0);
    kyber_shake256_prf(buf_e, sizeof(buf_e), seed, 1);
    kyber_shake256_prf(buf_r, sizeof(buf_r), seed, 2);
    kyber_shake256_prf(buf_e1, sizeof(buf_e1), seed, 3);
    kyber_shake256_prf(buf_e2, sizeof(buf_e2), seed, 4);

    // Convert PRF output to polynomials with centered binomial distribution
    poly_cbd_eta1(&s, buf_s);
    poly_cbd_eta1(&e, buf_e);
    poly_cbd_eta1(&r, buf_r);
    poly_cbd_eta1(&e1, buf_e1);
    poly_cbd_eta1(&e2, buf_e2);

    // Print results
    print_poly("s", &s);
    print_poly("e", &e);
    print_poly("r", &r);
    print_poly("e1", &e1);
    print_poly("e2", &e2);

    ESP_LOGI(TAG, "Noise generation complete.\n");
 while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
}
}
