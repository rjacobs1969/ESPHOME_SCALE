import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import sensor
from esphome.const import CONF_CLK_PIN, CONF_GAIN, UNIT_EMPTY, ICON_SCALE

hx711_dual_ns = cg.esphome_ns.namespace("hx711_dual")
HX711DualSensor = hx711_dual_ns.class_(
    "HX711DualSensor", sensor.Sensor, cg.PollingComponent
)

CONF_DOUT_PIN_A = "dout_pin_a"
CONF_DOUT_PIN_B = "dout_pin_b"

CONFIG_SCHEMA = (
    sensor.sensor_schema(
        HX711DualSensor,
        unit_of_measurement=UNIT_EMPTY,
        icon=ICON_SCALE,
        accuracy_decimals=0,
    )
    .extend(
        {
            cv.Required(CONF_DOUT_PIN_A): pins.internal_gpio_input_pin_schema,
            cv.Required(CONF_DOUT_PIN_B): pins.internal_gpio_input_pin_schema,
            cv.Required(CONF_CLK_PIN): pins.internal_gpio_output_pin_schema,
            cv.Optional(CONF_GAIN, default=128): cv.one_of(128, 32, 64, int=True),
        }
    )
    .extend(cv.polling_component_schema("17ms"))
)


async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
    cg.add(var.set_dout_pin_a(await cg.gpio_pin_expression(config[CONF_DOUT_PIN_A])))
    cg.add(var.set_dout_pin_b(await cg.gpio_pin_expression(config[CONF_DOUT_PIN_B])))
    cg.add(var.set_clk_pin(await cg.gpio_pin_expression(config[CONF_CLK_PIN])))
    cg.add(var.set_gain(config[CONF_GAIN]))
