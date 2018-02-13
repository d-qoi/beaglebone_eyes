#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/spi/spi.h>
#include <linux/sysfs.h>
#include <linux/types.h>
#include <linux/gpio.h>

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

/* SPI commands */

#define BIT18_SELECT (0x)