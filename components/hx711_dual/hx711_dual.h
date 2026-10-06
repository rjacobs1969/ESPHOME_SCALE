#pragma once

#include "esphome/core/component.h"
#include "esphome/core/gpio.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace hx711_dual {

// Two HX711 chips sharing one clock line; publishes the sum of both raw values.
class HX711DualSensor : public sensor::Sensor, public PollingComponent {
 public:
  void set_dout_pin_a(GPIOPin *pin) { dout_a_ = pin; }
  void set_dout_pin_b(GPIOPin *pin) { dout_b_ = pin; }
  void set_clk_pin(GPIOPin *pin) { clk_ = pin; }
  void set_gain(int gain) { gain_ = gain; }

  void setup() override;
  void dump_config() override;
  void update() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

 protected:
  GPIOPin *dout_a_{nullptr};
  GPIOPin *dout_b_{nullptr};
  GPIOPin *clk_{nullptr};
  int gain_{128};
};

}  // namespace hx711_dual
}  // namespace esphome
