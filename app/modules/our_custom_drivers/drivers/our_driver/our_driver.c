#define DT_DRV_COMPAT our_driver

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/gpio.h>

#include <our_driver/our_driver.h>

struct our_driver_config {
    struct gpio_dt_spec led;
};

struct our_driver_data {
    bool activated;
    bool enabled;
};

static int our_driver_init(const struct device* dev) {
    const struct our_driver_config *config = dev->config;
    struct our_driver_data   *data = dev->data;

    if (!gpio_is_ready_dt(&config->led)) {
        return -ENODEV;
    }

    data->enabled = true;
    return gpio_pin_configure_dt(&config->led, GPIO_OUTPUT_INACTIVE);
}

int our_driver_set_enabled(const struct device *dev, bool enabled) {
    const struct our_driver_config *config = dev->config;
    struct our_driver_data *data = dev->data;

    data->enabled = enabled;
    if (!enabled) {
        data->activated = false;
        gpio_pin_set_dt(&config->led, 0);
    }
    return 0;
}

static int our_driver_sample_fetch(const struct device *dev, enum sensor_channel chan) {
    struct our_driver_data* data = dev->data;
    const struct our_driver_config *config = dev->config;

    if (data->enabled) {
        data->activated = true;
        gpio_pin_set_dt(&config->led, 1);
    }
    return 0;
}

static int our_driver_channel_get(const struct device *dev, enum sensor_channel chan, struct sensor_value *val) {
    struct our_driver_data* data = dev->data;
    const struct our_driver_config *config = dev->config;

    val->val1 = data->activated ? 1 : 0;
    val->val2 = 0;

    if (data->enabled) {
        data->activated = false;
        gpio_pin_set_dt(&config->led, 0);
    }
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


