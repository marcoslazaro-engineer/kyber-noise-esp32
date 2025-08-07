#include <stddef.h>
#include <stdint.h>
#include "esp_random.h"

void randombytes(uint8_t *buf, size_t len) {
    esp_fill_random(buf, len);
}
