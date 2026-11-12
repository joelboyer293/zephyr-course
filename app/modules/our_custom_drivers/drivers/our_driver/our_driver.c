#define DT_DRV_COMPAT our_driver

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/gpio.h>

struct our_driver_config {
    struct gpio_dt_spec led;
};

struct our_driver_data {
    bool activated;
};

static int our_driver_init(const struct device* dev) {
    const struct our_driver_config *config = dev->config;

    if (!gpio_is_ready_dt(&config->led)) {
        return -ENODEV;
    }
    return gpio_pin_configure_dt(&config->led, GPIO_OUTPUT_INACTIVE);
}

static int our_driver_sample_fetch(const struct device *dev, enum sensor_channel chan) {
    struct our_driver_data* data = dev->data;
    const struct our_driver_config *config = dev->config;

    data->activated = true;
    gpio_pin_set_dt(&config->led, 1);
    return 0;
}

static int our_driver_channel_get(const struct device *dev, enum sensor_channel chan, struct sensor_value *val) {
    struct our_driver_data* data = dev->data;
    const struct our_driver_config *config = dev->config;

    data->activated = false;
    gpio_pin_set_dt(&config->led, 0);
    return 0;
}

static DEVICE_API(sensor, our_driver_api) = {
    .sample_fetch = our_driver_sample_fetch,
    .channel_get  = our_driver_channel_get
};


#define OUR_DRIVER_DEFINE(inst)                           \
    static struct our_driver_data data_##inst;            \
    static const struct our_driver_config cfg_##inst = {  \
        .led = GPIO_DT_SPEC_INST_GET(inst, led_gpios),    \
    };                                                    \
    DEVICE_DT_INST_DEFINE(inst,                           \
            our_driver_init, NULL,                        \
            &data_##inst, &cfg_##inst,                    \
            POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY,     \
            &our_driver_api)



DT_INST_FOREACH_STATUS_OKAY(OUR_DRIVER_DEFINE)


