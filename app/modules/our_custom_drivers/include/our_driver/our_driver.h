#ifndef OUR_DRIVER_H
#define OUR_DRIVER_H

#include <zephyr/device.h>

int our_driver_set_enabled(const struct device* dev, bool enabled);


#endif
