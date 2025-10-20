// #include <kernel.h>
// #include <drivers/gpio.h>
// #include <sys/printk.h>

// /* 1000 msec = 1 sec */
// #define SLEEP_TIME_MS   1000

// /* The devicetree node identifier for the "led0" alias. */
// #define LED0_NODE DT_ALIAS(led0)

// /* Build error here means the board has no LED0 alias defined */
// static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

// int main(void)
// {
// 	int ret;
// 	bool led_state = true;

// 	if (!device_is_ready(led.port)) {
// 		printk("Error: LED device is not ready\n");
// 		return 0;
// 	}

// 	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
// 	if (ret < 0) {
// 		printk("Error %d: failed to configure LED pin\n", ret);
// 		return 0;
// 	}

// 	printk("Starting LED blink example\n");

// 	while (1) {
// 		ret = gpio_pin_toggle_dt(&led);
// 		if (ret < 0) {
// 			printk("Error %d: failed to toggle LED\n", ret);
// 			return 0;
// 		}

// 		led_state = !led_state;
// 		printk("LED state: %s\n", led_state ? "ON" : "OFF");
// 		k_msleep(SLEEP_TIME_MS);
// 	}
// 	return 0;
// }
