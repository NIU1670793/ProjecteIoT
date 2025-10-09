// // /* main.c - Application main entry point */

// // /*
// //  * Copyright (c) 2015-2016 Intel Corporation
// //  *
// //  * SPDX-License-Identifier: Apache-2.0
// //  */

// #include <zephyr/types.h>
// #include <stddef.h>
// #include <sys/printk.h>
// #include <sys/util.h>
// #include <kernel.h>
// #include <drivers/gpio.h>

// #include <bluetooth/bluetooth.h>
// #include <bluetooth/hci.h>
// #include <sys/types.h>

// // #define DEVICE_NAME CONFIG_BT_DEVICE_NAME
// // #define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

// // /*
// //  * Set Advertisement data. Based on the Eddystone specification:
// //  * https://github.com/google/eddystone/blob/master/protocol-specification.md
// //  * https://github.com/google/eddystone/tree/master/eddystone-url
// //  */
// // static const struct bt_data ad[] = {
// // 	BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
// // 	BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0xaa, 0xfe),
// // 	BT_DATA_BYTES(BT_DATA_SVC_DATA16,
// // 		      0xaa, 0xfe, /* Eddystone UUID */
// // 		      0x10, /* Eddystone-URL frame type */
// // 		      0x00, /* Calibrated Tx power at 0m */
// // 		      0x00, /* URL Scheme Prefix http://www. */
// // 		      'z', 'e', 'p', 'h', 'y', 'r',
// // 		      'p', 'r', 'o', 'j', 'e', 'c', 't',
// // 		      0x08) /* .org */
// // };

// // /* Set Scan Response data */
// // static const struct bt_data sd[] = {
// // 	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
// // };

// // static void bt_ready(int err)
// // {
// // 	char addr_s[BT_ADDR_LE_STR_LEN];
// // 	bt_addr_le_t addr = {0};
// // 	size_t count = 1;

// // 	if (err) {
// // 		printk("Bluetooth init failed (err %d)\n", err);
// // 		return;
// // 	}

// // 	printk("Bluetooth initialized\n");

// // 	/* Start advertising */
// // 	err = bt_le_adv_start(BT_LE_ADV_NCONN_IDENTITY, ad, ARRAY_SIZE(ad),
// // 			      sd, ARRAY_SIZE(sd));
// // 	if (err) {
// // 		printk("Advertising failed to start (err %d)\n", err);
// // 		return;
// // 	}


// // 	/* For connectable advertising you would use
// // 	 * bt_le_oob_get_local().  For non-connectable non-identity
// // 	 * advertising an non-resolvable private address is used;
// // 	 * there is no API to retrieve that.
// // 	 */

// // 	bt_id_get(&addr, &count);
// // 	bt_addr_le_to_str(&addr, addr_s, sizeof(addr_s));

// // 	printk("Beacon started, advertising as %s\n", addr_s);
// // }

// // void main(void)
// // {
// // 	int err;

// // 	printk("Starting Beacon Demo\n");

// // 	/* Initialize the Bluetooth Subsystem */
// // 	err = bt_enable(bt_ready);
// // 	if (err) {
// // 		printk("Bluetooth init failed (err %d)\n", err);
// // 	}



#include <zephyr.h>
#include <device.h>
#include <drivers/i2c.h>
#include <logging/log.h>
#include <zephyr/types.h>
#include <stddef.h>
#include <sys/printk.h>
#include <sys/util.h>
#include <kernel.h>
#include <drivers/gpio.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <sys/types.h>

LOG_MODULE_REGISTER(veml, LOG_LEVEL_INF);

// Constants del sensor
#define VEML6040_I2C_ADDR  0x10
#define VEML6040_CONF      0x00
#define VEML6040_RED       0x08
#define VEML6040_GREEN     0x09
#define VEML6040_BLUE      0x0A
#define VEML6040_WHITE     0x0B

void main(void)
{
    const struct device *i2c_dev;
    uint8_t data[2];
    uint16_t red, green, blue, white;

    // Obtenim el dispositiu I2C (definit al .overlay)
    i2c_dev = device_get_binding("I2C_0");
    if (!i2c_dev) {
        LOG_ERR("No s'ha trobat el dispositiu I2C_0");
        return;
    }

    // Configuració bàsica del sensor
    uint8_t cmd[3] = { VEML6040_CONF, 0x00, 0x00 };
    if (i2c_write(i2c_dev, cmd, sizeof(cmd), VEML6040_I2C_ADDR) < 0) {
        LOG_ERR("Error configurant el sensor VEML6040");
        return;
    }

    LOG_INF("VEML6040 inicialitzat correctament!");

    while (1) {
        // Llegim cada canal de color
        i2c_burst_read(i2c_dev, VEML6040_I2C_ADDR, VEML6040_RED, data, 2);
        red = (data[1] << 8) | data[0];

        i2c_burst_read(i2c_dev, VEML6040_I2C_ADDR, VEML6040_GREEN, data, 2);
        green = (data[1] << 8) | data[0];

        i2c_burst_read(i2c_dev, VEML6040_I2C_ADDR, VEML6040_BLUE, data, 2);
        blue = (data[1] << 8) | data[0];

        i2c_burst_read(i2c_dev, VEML6040_I2C_ADDR, VEML6040_WHITE, data, 2);
        white = (data[1] << 8) | data[0];

        LOG_INF("Color -> R:%u G:%u B:%u W:%u", red, green, blue, white);

        k_sleep(K_SECONDS(1));
    }
}
