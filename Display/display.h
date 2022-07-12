#pragma once

#include <stm32f1xx_hal.h>
#include <stdio.h>

#define CHAR_LENGTH 8
#define CHAR_SPACER 2

void lcd_init(SPI_HandleTypeDef *_spi_handle);
void lcd_clear(void);
void lcd_draw_bytemap(uint8_t *bytemap, uint8_t width, uint8_t height, uint8_t x, uint8_t y);
void lcd_draw_bitmap(uint8_t *bitmap, uint8_t width, uint8_t height, uint8_t x, uint8_t y);
void lcd_update(void);
void lcd_backlight(uint8_t enabled);
void lcd_set_cursor_x(uint8_t x);
void lcd_set_cursor_y(uint8_t y);
void lcd_set_cursor(uint8_t x, uint8_t y);
