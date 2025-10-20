// #include <zephyr.h>
// #include <sys/printk.h>
// #include <logging/log.h>
// #include <device.h>
// #include <drivers/i2c.h>
// #include <bluetooth/bluetooth.h>
// #include <bluetooth/hci.h>
// #include <bluetooth/gatt.h>

// LOG_MODULE_REGISTER(veml_ble, LOG_LEVEL_INF);

// // ======== CONSTANTS SENSOR VEML6040 ========
// #define VEML6040_I2C_ADDR  0x10
// #define VEML6040_CONF      0x00
// #define VEML6040_RED       0x08
// #define VEML6040_GREEN     0x09
// #define VEML6040_BLUE      0x0A
// #define VEML6040_WHITE     0x0B

// // ======== VARIABLES ========
// static const struct device *i2c_dev;
// static uint8_t color_data[8]; // R,G,B,W (2 bytes cadascun)

// // ======== BLE UUIDs PERSONALITZATS ========
// #define BT_UUID_COLOR_SERVICE   BT_UUID_DECLARE_128(BT_UUID_128_ENCODE(0x12345678,0x1234,0x5678,0x1234,0x56789abcdef0))
// #define BT_UUID_COLOR_CHAR      BT_UUID_DECLARE_128(BT_UUID_128_ENCODE(0xabcdef01,0x2345,0x6789,0xabcd,0xef0123456789))

// // ======== FUNCIONS BLE ========

// // Funció de lectura de la característica
// static ssize_t read_color(struct bt_conn *conn,
//                           const struct bt_gatt_attr *attr,
//                           void *buf, uint16_t len, uint16_t offset)
// {
//     return bt_gatt_attr_read(conn, attr, buf, len, offset, color_data, sizeof(color_data));
// }

// // Definició del servei GATT
// BT_GATT_SERVICE_DEFINE(color_svc,
//     BT_GATT_PRIMARY_SERVICE(BT_UUID_COLOR_SERVICE),
//     BT_GATT_CHARACTERISTIC(BT_UUID_COLOR_CHAR,
//                            BT_GATT_CHRC_READ,
//                            BT_GATT_PERM_READ,
//                            read_color, NULL, NULL),
// );

// // Callback Bluetooth
// static void bt_ready(int err)
// {
//     if (err) {
//         LOG_ERR("Error iniciant Bluetooth (%d)", err);
//         return;
//     }

//     LOG_INF("Bluetooth inicialitzat correctament!");
// }

// // ======== FUNCIONS SENSOR ========

// static int veml6040_init(void)
// {
//     i2c_dev = device_get_binding("I2C_0");
//     if (!i2c_dev) {
//         LOG_ERR("No s'ha trobat el dispositiu I2C_0");
//         return -1;
//     }

//     uint8_t cmd[3] = { VEML6040_CONF, 0x00, 0x00 };
//     if (i2c_write(i2c_dev, cmd, sizeof(cmd), VEML6040_I2C_ADDR) < 0) {
//         LOG_ERR("Error configurant el sensor VEML6040");
//         return -1;
//     }

//     LOG_INF("VEML6040 inicialitzat correctament!");
//     return 0;
// }

// static void veml6040_read_colors(void)
// {
//     uint8_t data[2];
//     uint16_t red, green, blue, white;

//     i2c_burst_read(i2c_dev, VEML6040_I2C_ADDR, VEML6040_RED, data, 2);
//     red = (data[1] << 8) | data[0];

//     i2c_burst_read(i2c_dev, VEML6040_I2C_ADDR, VEML6040_GREEN, data, 2);
//     green = (data[1] << 8) | data[0];

//     i2c_burst_read(i2c_dev, VEML6040_I2C_ADDR, VEML6040_BLUE, data, 2);
//     blue = (data[1] << 8) | data[0];

//     i2c_burst_read(i2c_dev, VEML6040_I2C_ADDR, VEML6040_WHITE, data, 2);
//     white = (data[1] << 8) | data[0];

//     // Actualitzem l’array per BLE
//     color_data[0] = red & 0xFF;
//     color_data[1] = red >> 8;
//     color_data[2] = green & 0xFF;
//     color_data[3] = green >> 8;
//     color_data[4] = blue & 0xFF;
//     color_data[5] = blue >> 8;
//     color_data[6] = white & 0xFF;
//     color_data[7] = white >> 8;

//     LOG_INF("Color -> R:%u G:%u B:%u W:%u", red, green, blue, white);
// }

// // ======== MAIN ========

// void main(void)
// {
//     LOG_INF("Inicialitzant VEML6040 + BLE...");

//     if (veml6040_init() < 0) return;

//     int err = bt_enable(bt_ready);
//     if (err) {
//         LOG_ERR("Error activant BLE (%d)", err);
//         return;
//     }

//     LOG_INF("Esperant connexió BLE...");

//     while (1) {
//         veml6040_read_colors();
//         k_sleep(K_SECONDS(1));
//     }
// }
