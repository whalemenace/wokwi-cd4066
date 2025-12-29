#include <stdbool.h>
#include <stdint.h>
#include "wokwi-api.h"

typedef struct {
  pin_t y;
  pin_t z;
  pin_t e;
  bool enabled;
  bool driving;
} channel_t;

static channel_t ch[4];

static void apply_channel(int i) {
  channel_t *c = &ch[i];

  uint32_t en = pin_read(c->e);
  if (!en) {
    if (c->enabled) {
      c->enabled = false;
      // Release the pins back to the circuit
      pin_mode(c->y, INPUT);
      pin_mode(c->z, INPUT);
    }
    return;
  }

  // Enabled: "short" Y and Z together.
  // We approximate a short by driving both pins to a combined value.
  // LOW dominates (useful for pullup->ground button press behavior).
  uint32_t vy = pin_read(c->y);
  uint32_t vz = pin_read(c->z);
  uint32_t v = (vy == HIGH && vz == HIGH) ? HIGH : LOW;

  c->enabled = true;
  c->driving = true;
  pin_mode(c->y, OUTPUT);
  pin_mode(c->z, OUTPUT);
  pin_write(c->y, v);
  pin_write(c->z, v);
  c->driving = false;
}

static void on_pin_change(void *user_data, pin_t pin, uint32_t value) {
  int i = (int)(uintptr_t)user_data;
  channel_t *c = &ch[i];

  // Prevent feedback loops from our own pin_write() calls
  if (c->driving) return;

  apply_channel(i);
}

void chip_init(void) {
  // Channel 1
  ch[0].y = pin_init("1Y", INPUT);
  ch[0].z = pin_init("1Z", INPUT);
  ch[0].e = pin_init("1E", INPUT);

  // Channel 2
  ch[1].y = pin_init("2Y", INPUT);
  ch[1].z = pin_init("2Z", INPUT);
  ch[1].e = pin_init("2E", INPUT);

  // Channel 3
  ch[2].y = pin_init("3Y", INPUT);
  ch[2].z = pin_init("3Z", INPUT);
  ch[2].e = pin_init("3E", INPUT);

  // Channel 4 (unused in your project, but implemented)
  ch[3].y = pin_init("4Y", INPUT);
  ch[3].z = pin_init("4Z", INPUT);
  ch[3].e = pin_init("4E", INPUT);

  // VCC/GND pins exist for diagram correctness (not used in logic here)
  pin_init("VCC", INPUT);
  pin_init("GND", INPUT);

  // Watch pins so the switch reacts when E changes or when either side changes
  for (int i = 0; i < 4; i++) {
    pin_watch_config_t cfg = {
      .user_data = (void*)(uintptr_t)i,
      .edge = BOTH,
      .pin_change = on_pin_change
    };
    pin_watch(ch[i].e, &cfg);
    pin_watch(ch[i].y, &cfg);
    pin_watch(ch[i].z, &cfg);
  }
}
