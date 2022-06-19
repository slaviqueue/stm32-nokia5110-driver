#if !defined(DISPLAY_H)
#define DISPLAY_H

#include <stm32f1xx_hal.h>
#include <stdio.h>

void lcd_init(SPI_HandleTypeDef *_spi_handle);
void lcd_clear();
void lcd_draw(uint8_t *data, uint8_t size);
void lcd_backlight(uint8_t enabled);

#endif // DISPLAY_H
