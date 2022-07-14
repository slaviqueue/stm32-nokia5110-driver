#pragma once

#include <stm32f1xx_hal.h>
#include <stdio.h>

#define FONT_LENGTH 128
#define LETTER_LENGTH 8

void lcd_set_spi_handle(SPI_HandleTypeDef *_spi_handle);

void lcd_set_chip_enable_pin(GPIO_TypeDef *port, uint16_t pin);
void lcd_set_light_pin(GPIO_TypeDef *port, uint16_t pin);
void lcd_set_reset_pin(GPIO_TypeDef *port, uint16_t pin);
void lcd_set_data_command_pin(GPIO_TypeDef *port, uint16_t pin);

void lcd_set_contrast(uint8_t contrast);
void lcd_set_temp_control(uint8_t temp_control);
void lcd_set_bias_mode(uint8_t bias_mode);

void lcd_set_font(uint8_t font[128][8]);
void lcd_clear(void);
void lcd_draw_bytemap(uint8_t *bytemap, uint8_t width, uint8_t height, uint8_t x, uint8_t y);
void lcd_set_pixel(uint8_t x, uint8_t y, uint8_t val);
void lcd_draw_bitmap(uint8_t *bitmap, uint8_t width, uint8_t height, uint8_t x, uint8_t y);
void lcd_update(void);
void lcd_backlight(uint8_t enabled);
void lcd_print(char *text, uint8_t x, uint8_t y);
