#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_timer.h"
#include "driver/gpio.h"

#include "params.h"
#include "poly.h"
#include "cbd.h"
#include "ntt.h"
#include "randombytes.h"
#include "symmetric.h"

// ====== Side-channel trigger (GPIO2) ======
#ifndef TRIGGER_GPIO
#define TRIGGER_GPIO 2
#endif

static inline void trigger_init(void) {
    gpio_config_t io = {
        .pin_bit_mask = 1ULL << TRIGGER_GPIO,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io);
    gpio_set_level(TRIGGER_GPIO, 0);
}
static inline void trig_hi(void){ gpio_set_level(TRIGGER_GPIO, 1); }
static inline void trig_lo(void){ gpio_set_level(TRIGGER_GPIO, 0); }

// ====== Pretty printing ======
static void print_poly(const char *name, const poly *p) {
    printf("%s:\n", name);
    for (int i = 0; i < KYBER_N; i++) {
        printf("%d", p->coeffs[i]);
        if ((i+1) % 16 == 0 || i+1 == KYBER_N) printf("\n");
        else printf(" ");
    }
    printf("\n");
}

void app_main(void) {
    // --- Init trigger
    trigger_init();

    // --- Seed for noise PRF (32 bytes). Top-tier: randombytes() should map a TRNG postprocesado (p. ej., SHAKE-based DRBG).
    uint8_t seed[KYBER_SYMBYTES];
    randombytes(seed, sizeof(seed));

    printf("# Noise seed (32B): ");
    for (size_t i = 0; i < sizeof(seed); i++) printf("%02X", seed[i]);
    printf("\n");

    // --- Buffers sized per Kyber (η1 = 3, η2 = 2 for Kyber512)
    enum { LEN_ETA1 = KYBER_ETA1 * KYBER_N / 4, LEN_ETA2 = KYBER_ETA2 * KYBER_N / 4 };
    static uint8_t buf_s[LEN_ETA1];
    static uint8_t buf_e[LEN_ETA1];
    static uint8_t buf_r[LEN_ETA2];
    static uint8_t buf_e1[LEN_ETA2];
    static uint8_t buf_e2[LEN_ETA2];

    poly s, e, r, e1, e2;

    // --- Measure per-poly timings (PRF + CBD), with trigger windows
    int64_t t_total0 = esp_timer_get_time();

    // s (η1, nonce 0)
    int64_t t0 = esp_timer_get_time();
    trig_hi();
    kyber_shake256_prf(buf_s, sizeof(buf_s), seed, 0);
    poly_cbd_eta1(&s, buf_s);
    trig_lo();
    int64_t t1 = esp_timer_get_time();
    printf("# t_s_us=%" PRId64 "\n", (t1 - t0));

    // e (η1, nonce 1)
    t0 = esp_timer_get_time();
    trig_hi();
    kyber_shake256_prf(buf_e, sizeof(buf_e), seed, 1);
    poly_cbd_eta1(&e, buf_e);
    trig_lo();
    t1 = esp_timer_get_time();
    printf("# t_e_us=%" PRId64 "\n", (t1 - t0));

    // r (η2, nonce 2)
    t0 = esp_timer_get_time();
    trig_hi();
    kyber_shake256_prf(buf_r, sizeof(buf_r), seed, 2);
    poly_cbd_eta2(&r, buf_r);
    trig_lo();
    t1 = esp_timer_get_time();
    printf("# t_r_us=%" PRId64 "\n", (t1 - t0));

    // e1 (η2, nonce 3)
    t0 = esp_timer_get_time();
    trig_hi();
    kyber_shake256_prf(buf_e1, sizeof(buf_e1), seed, 3);
    poly_cbd_eta2(&e1, buf_e1);
    trig_lo();
    t1 = esp_timer_get_time();
    printf("# t_e1_us=%" PRId64 "\n", (t1 - t0));

    // e2 (η2, nonce 4)
    t0 = esp_timer_get_time();
    trig_hi();
    kyber_shake256_prf(buf_e2, sizeof(buf_e2), seed, 4);
    poly_cbd_eta2(&e2, buf_e2);
    trig_lo();
    t1 = esp_timer_get_time();
    printf("# t_e2_us=%" PRId64 "\n", (t1 - t0));

    int64_t t_total1 = esp_timer_get_time();
    printf("# t_total_noise_us=%" PRId64 "\n", (t_total1 - t_total0));

    // --- Dump polys (determinado por seed)
    print_poly("s (eta1)",  &s);
    print_poly("e (eta1)",  &e);
    print_poly("r (eta2)",  &r);
    print_poly("e1 (eta2)", &e1);
    print_poly("e2 (eta2)", &e2);

    // Mantén vivo (evita watchdog si quieres) con delay largo
    while (1) vTaskDelay(pdMS_TO_TICKS(1000));
}
