#include "hx711_dual.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace hx711_dual {

static const char *const TAG = "hx711_dual";

void HX711DualSensor::setup() {
  dout_a_->setup();
  dout_b_->setup();
  clk_->setup();
  clk_->digital_write(false);
}

void HX711DualSensor::dump_config() {
  LOG_SENSOR("", "HX711 Dual", this);
  LOG_PIN("  DOUT A Pin: ", dout_a_);
  LOG_PIN("  DOUT B Pin: ", dout_b_);
  LOG_PIN("  CLK Pin: ", clk_);
  ESP_LOGCONFIG(TAG, "  Gain: %d", gain_);
  LOG_UPDATE_INTERVAL(this);
}

static int32_t sign_extend24(uint32_t v) {
  if (v & 0x800000) v |= 0xFF000000;
  return (int32_t) v;
}

void HX711DualSensor::update() {
  // Both chips must have a conversion ready, otherwise clocking would discard it
  if (dout_a_->digital_read() || dout_b_->digital_read()) {
    ESP_LOGV(TAG, "Not both HX711 ready yet");
    return;
  }

  uint32_t a = 0, b = 0;
  {
    InterruptLock lock;
    for (int i = 0; i < 24; i++) {
      clk_->digital_write(true);
      delayMicroseconds(1);
      a = (a << 1) | (dout_a_->digital_read() ? 1 : 0);
      b = (b << 1) | (dout_b_->digital_read() ? 1 : 0);
      clk_->digital_write(false);
      delayMicroseconds(1);
    }
    int extra = gain_ == 128 ? 1 : (gain_ == 32 ? 2 : 3);
    for (int i = 0; i < extra; i++) {
      clk_->digital_write(true);
      delayMicroseconds(1);
      clk_->digital_write(false);
      delayMicroseconds(1);
    }
  }

  int32_t sum = sign_extend24(a) + sign_extend24(b);
  ESP_LOGV(TAG, "A=%d B=%d sum=%d", sign_extend24(a), sign_extend24(b), sum);
  this->publish_state((float) sum);
}

}  // namespace hx711_dual
}  // namespace esphome
