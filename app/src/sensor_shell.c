#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/shell/shell.h>
#include <our_driver/our_driver.h>

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

// l7-task2 command
static int cmd_set(const struct shell *sh, size_t argc, char **argv) {
    int error = 0;

    if (argc < 2) { // Extra check on top of SHELL_CMD_ARG
        shell_error(sh, "missing argument: sensor set <0|1>");
        return -EINVAL;
    }

    unsigned long value = shell_strtoul(argv[1], 10, &error);
    if (error != 0) {
        shell_error(sh, "invalid number: '%s'", argv[1]);
        return -EINVAL;
    }
    if (value > 1) { // Can only set true or false for my custom command
        shell_error(sh, "out of range: %lu (expected 0 or 1)", value);
        return -ERANGE;
    }

    if (!device_is_ready(sensor_dev)) {
        shell_error(sh, "%s not ready", sensor_dev->name);
        return -ENODEV;
    }

    int result = our_driver_set_enabled(sensor_dev, value == 1);
    if (result < 0) {
        shell_error(sh, "our_driver_set_enabled failed: %d", result);
        return result;
    }

    shell_print(sh, "LED %s", value ? "enabled" : "disabled");
    return 0;
}

// The sensor subcommands table
SHELL_STATIC_SUBCMD_SET_CREATE(sub_sensor,
    SHELL_CMD(fetch, NULL, "Fetch a sample (sensor_sample_fetch)", cmd_fetch),
    SHELL_CMD(read,  NULL, "Read channel value (sensor_channel_get)", cmd_read),
    SHELL_CMD(info,  NULL, "Show device name and ready state", cmd_info),

    SHELL_CMD_ARG(set, NULL, "Enable/disable the LED: set <0|1>", cmd_set, 2, 0), // mandatory = 2 ("set" + value), optional = 0
    SHELL_SUBCMD_SET_END
);

// Root command with no handler, points to subcommand table
SHELL_CMD_REGISTER(sensor, &sub_sensor, "Our LED sensor commands", NULL);
