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
#include <bluetooth/gatt.h>
#include <sys/types.h>


LOG_MODULE_REGISTER(veml_ble, LOG_LEVEL_INF);

// Constants del sensor
#define VEML6040_I2C_ADDR  0x10
#define VEML6040_CONF      0x03
#define VEML6040_RED       0x08
#define VEML6040_GREEN     0x09
#define VEML6040_BLUE      0x0A
#define VEML6040_WHITE     0x0B

static const struct device *i2c_dev;
static uint8_t color_data[8]; // R,G,B,W (2 bytes cadascun)

#define BT_UUID_COLOR_SERVICE   BT_UUID_DECLARE_128(BT_UUID_128_ENCODE(0x12345678,0x1234,0x5678,0x1234,0x56789abcdef0))
#define BT_UUID_COLOR_CHAR      BT_UUID_DECLARE_128(BT_UUID_128_ENCODE(0xabcdef01,0x2345,0x6789,0xabcd,0xef0123456789))

static bool notify_enabled = false;

static void color_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    notify_enabled = (value == BT_GATT_CCC_NOTIFY);
    LOG_INF("Notificacions %s", notify_enabled ? "activades" : "desactivades");
}

static ssize_t read_color(struct bt_conn *conn,
                          const struct bt_gatt_attr *attr,
                          void *buf, uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset, color_data, sizeof(color_data));
}
static const struct bt_data sd[] = {
    BT_DATA(BT_DATA_NAME_COMPLETE, "VEML_RGBW", sizeof("VEML_RGBW") - 1),
};


static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID128_ALL,
                  0xf0,0xde,0xbc,0x9a,0x78,0x56,0x34,0x12,
                  0x34,0x12,0x78,0x56,0x34,0x12,0x56,0x12) // El teu UUID 128-bit
};


BT_GATT_SERVICE_DEFINE(color_svc,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_COLOR_SERVICE),
    BT_GATT_CHARACTERISTIC(BT_UUID_COLOR_CHAR,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_READ,
                           read_color, NULL, NULL),
    BT_GATT_CCC(color_ccc_cfg_changed, 
                   BT_GATT_PERM_READ | BT_GATT_PERM_WRITE)
);

static void bt_ready(int err)
{
    if (err) {
        LOG_ERR("Error iniciant Bluetooth (%d)", err);
        return;
    }
    bt_set_name("VEML_RGBW");  // Nom visible a nRF Connect


    LOG_INF("Bluetooth inicialitzat correctament!", "Inf");
    bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
    LOG_INF("Publicant servei GATT de color (RGBW)...", "Inf");
}

static int veml6040_init(void)
{
    i2c_dev = device_get_binding("I2C_0");
    if (!i2c_dev) {
        LOG_ERR("No s'ha trobat el dispositiu I2C_0", "Error");
        return -1;
    }

    uint8_t cmd[3] = { VEML6040_CONF, 0x00, 0x00 };
    if (i2c_write(i2c_dev, cmd, sizeof(cmd), VEML6040_I2C_ADDR) < 0) {
        LOG_ERR("Error configurant el sensor VEML6040", "Error");
        return -1;
    }

    LOG_INF("VEML6040 inicialitzat correctament!", "Inf");
    return 0;
}

static void veml6040_read_colors(void)
{
    uint8_t data[2];
    uint16_t red, green, blue, white;

    i2c_burst_read(i2c_dev, VEML6040_I2C_ADDR, VEML6040_RED, data, 2);
    red = (data[1] << 8) | data[0];

    i2c_burst_read(i2c_dev, VEML6040_I2C_ADDR, VEML6040_GREEN, data, 2);
    green = (data[1] << 8) | data[0];

    i2c_burst_read(i2c_dev, VEML6040_I2C_ADDR, VEML6040_BLUE, data, 2);
    blue = (data[1] << 8) | data[0];

    i2c_burst_read(i2c_dev, VEML6040_I2C_ADDR, VEML6040_WHITE, data, 2);
    white = (data[1] << 8) | data[0];

    // Actualitzem l’array per BLE
    color_data[0] = red & 0xFF;
    color_data[1] = red >> 8;
    color_data[2] = green & 0xFF;
    color_data[3] = green >> 8;
    color_data[4] = blue & 0xFF;
    color_data[5] = blue >> 8;
    color_data[6] = white & 0xFF;
    color_data[7] = white >> 8;

    LOG_INF("Color -> R:%u G:%u B:%u W:%u", red, green, blue, white);

    // if (notigy_enabled) {
    //     bt_gatt_notify(NULL, &color_svc.attrs[1], color_data, sizeof(color_data));
    // }
}

void main(void)
{
    LOG_INF("Inicialitzant VEML6040 + BLE...", "Inf");

    if (veml6040_init() < 0 ) return;

    int err = bt_enable(bt_ready);
    if (err) {
        LOG_ERR("Error activant BLE (%d)", err);
        return; 
    } 

    while (1)
    {
        veml6040_read_colors();
        if(notify_enabled) {
            bt_gatt_notify(NULL, &color_svc.attrs[1], color_data, sizeof(color_data));
        }
        k_sleep(K_SECONDS(1));
    }
    
    

}
    
   