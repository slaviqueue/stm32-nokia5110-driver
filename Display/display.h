#if !defined(DISPLAY_H)
#define DISPLAY_H

#include <stm32f1xx_hal.h>
#include <stdio.h>

void lcd_init(SPI_HandleTypeDef *_spi_handle);
void lcd_clear(void);
void lcd_draw(uint8_t *data, uint8_t size);
void lcd_backlight(uint8_t enabled);
void lcd_set_cursor_x(uint8_t x);
void lcd_set_cursor_y(uint8_t y);
void lcd_set_cursor(uint8_t x, uint8_t y);

#endif // DISPLAY_H
