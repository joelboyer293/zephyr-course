#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/shell/shell.h>

static const struct device *const sensor_dev = DEVICE_DT_GET(DT_NODELABEL(our_led_sensor));

static int cmd_fetch(const struct shell *sh, size_t argc, char **argv) {
    if (!device_is_ready(sensor_dev)) {
        shell_error(sh, "%s not ready", sensor_dev->name);
        return -ENODEV;
    }

    int result = sensor_sample_fetch(sensor_dev);
    if (result < 0) {
        shell_error(sh, "sensor_sample_fetch failed: %d", result);
        return result;
    }

    shell_print(sh, "fetch OK");
    return 0;
}

static int cmd_read(const struct shell *sh, size_t argc, char **argv) {
    struct sensor_value val = {0};

    if (!device_is_ready(sensor_dev)) {
        shell_error(sh, "%s not ready", sensor_dev->name);
        return -ENODEV;
    }

    int value = sensor_channel_get(sensor_dev, SENSOR_CHAN_ALL, &val);
    if (value < 0) {
        shell_error(sh, "sensor_channel_get failed: %d", value);
        return value;
    }

    shell_print(sh, "value: %d.%06d", val.val1, val.val2);
    return 0;
}

static int cmd_info(const struct shell *sh, size_t argc, char **argv) {
    shell_print(sh, "name:  %s", sensor_dev->name);
    shell_print(sh, "ready: %s", device_is_ready(sensor_dev) ? "yes" : "no");
    return 0;
}

// The sensor subcommands table
SHELL_STATIC_SUBCMD_SET_CREATE(sub_sensor,
    SHELL_CMD(fetch, NULL, "Fetch a sample (sensor_sample_fetch)", cmd_fetch),
    SHELL_CMD(read,  NULL, "Read channel value (sensor_channel_get)", cmd_read),
    SHELL_CMD(info,  NULL, "Show device name and ready state", cmd_info),
    SHELL_SUBCMD_SET_END
);

// Root command with no handler, points to subcommand table
SHELL_CMD_REGISTER(sensor, &sub_sensor, "Our LED sensor commands", NULL);
