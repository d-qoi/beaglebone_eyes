/* @file twoScreenDriver.c
 * @author Alexander Hirschfeld
 * @date 2/10/2017
 * @breif A kernel module designed to control 2 screens over two SPI interfaces
 * @version v0.1
 * @liscense GPLv3
 */


/* Currently this is hard coded to work with the SSD1351 OLED controller 

Currently assuming that everyting is going to be passed in as a pixel buffer
each pixel will have 3 8 bit sections (will be trimmed down for transmitting)

Red Green Blue (all 8 bits)
*/


#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/spi/spi.h>
#include <linux/sysfs.h>
#include <linux/types.h>
#include <linux/gpio.h>

MODULE_LICENSE("GPLv3");
MODULE_AUTHOR("Alex Hirschfeld");
MODULE_DESCRIPTION("Two Screen SPI Driver");
MODULE_VERSION("0.1");

#define DRIVER_NAME "twoScreenDriver"

#define GPIO_TO_PIN(bank, pin) ((32*bank)+pin)

static struct spi_device *spi_dev_left;
static struct spi_device *spi_dev_right;

static uint32_t screen_size_left[2]; // will default to 128x128
static uint8_t ssl_count;
static uint32_t screen_size_right[2]; 
static uint8_t ssr_count;

static uint8_t *buffer_1_left;
static uint8_t *buffer_2_left;
static uint8_t *buffer_left; // holds pointer to current buffer.

static uint8_t *buffer_1_right;
static uint8_t *buffer_2_right;
static uint8_t *buffer_right;

static inline uint32_t create_message(uint32_t x, uint32_t y, uint8_t* buffer) {
	uint32_t msg;
	msg = (buffer[0]|(0xFB)) << 10; // is shifted 12 because ignoring first two bits
	msg |= (buffer[1]|(0xFB)) << 4; // is shifted 6 because ignoring first two bits
	msg |= (buffer[2]|0xFB) >> 2; // only need 6 bits.
	return mgs;
}

module_param_array(screen_size_left, uint32_t, ssl_count, S_IRUSR | S_IRGRP | S_IROTH);
MODULE_PARAM_DESC(screen_size_left, "row, col");
module_param_array(screen_size_right, uint32_t, ssr_count, S_IRUSR | S_IRGRP | S_IROTH);
MODULE_PARAM_DESC(screen_size_right, "row, col");

static int __init twoScreenDriver_init(void) {
	printk(KERN_INFO "%s: Initialization\n", DRIVER_NAME);
	if (ssl_count != 2) {
		printk(KERN_INFO DRIVER_NAME "defaulting to 128x128 for left screen\n");
		screen_size_left[0] = 128;
		screen_size_left[1] = 128;
	}
	if (ssr_count != 2) {
		printk(KERN_INFO DRIVER_NAME "defaulting to 128x128 for left screen\n");
		screen_size_right[0] = 128;
		screen_size_right[1] = 128;
	}

	buffer_1_left = (uint8_t*)kmalloc(screen_size_left[0]*screen_size_left[1]*3, GPF_USER);
	buffer_2_left = (uint8_t*)kmalloc(screen_size_left[0]*screen_size_left[1]*3, GPF_USER);
	buffer_1_right = (uint8_t*)kmalloc(screen_size_right[0]*screen_size_right[1]*3, GPF_USER);
	buffer_2_right = (uint8_t*)kmalloc(screen_size_right[0]*screen_size_right[1]*3, GPF_USER);

	buffer_left = buffer_1_left;
	buffer_right = buffer_1_right;

	if (!buffer_1_left || !buffer_2_left || !buffer_1_right || !buffer_2_right) {
		kfree(buffer_1_left);
		kfree(buffer_2_left);
		kfree(buffer_1_right);
		kfree(buffer_2_right);
		printk(KERN_INFO DRIVER_NAME "Unable to allocate memory for some reason\n");
		return 1;
	}
	printk(KERN_INFO DRIVER_NAME "Buffers Allocated");
	
	
	return 0;
}

static void __exit twoScreenDriver_exit(void) {
	return 0;
}