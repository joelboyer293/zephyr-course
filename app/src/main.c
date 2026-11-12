#include "zephyr/drivers/sensor.h"
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <our_driver/our_driver.h>

/* The devicetree node identifier for the "led0" alias. */
#define LED_NODE DT_ALIAS(app_led)

static const struct device *const led_dev = DEVICE_DT_GET(DT_NODELABEL(our_led_sensor));

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void) {
    struct sensor_value val;
    int count = 0;

    if (!device_is_ready(led_dev)) {
        LOG_ERR("our_driver not ready");
        return 0;
    }

    while (1) {
        sensor_sample_fetch(led_dev);                        /* LED on (if enabled) */
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);
        sensor_channel_get(led_dev, SENSOR_CHAN_ALL, &val);  /* LED off */
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);

        count++;
        if (count % 5 == 0) {
            bool enable = (count / 5) % 2 == 0;
            our_driver_set_enabled(led_dev, enable);         /* extension API call */
            LOG_INF("LED %s", enable ? "enabled" : "disabled");
        }
    }
    return 0;
}
