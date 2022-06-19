#include "stm32f1xx_hal.h"

#define LCD_CHIP_ENABLE_PORT GPIOA
#define LCD_CHIP_ENABLE_PIN GPIO_PIN_4

#define LCD_LIGHT_PORT GPIOA
#define LCD_LIGHT_PIN GPIO_PIN_3

#define LCD_RESET_PORT GPIOA
#define LCD_RESET_PIN GPIO_PIN_2

#define LCD_DATA_COMMAND_PORT GPIOA
#define LCD_DATA_COMMAND_PIN GPIO_PIN_1

#define LCD_COMMAND_MODE GPIO_PIN_RESET
#define LCD_DATA_MODE GPIO_PIN_SET

#define LCD_SPI_TIMEOUT 1
#define PIXELS_AMOUNT 504

SPI_HandleTypeDef *spi_handle;

static void lcd_reset(void);
static void lcd_write_command(uint8_t data);
static void lcd_write_data(uint8_t *data, uint8_t size);
static void lcd_write_byte(uint8_t data);

void lcd_init(SPI_HandleTypeDef *_spi_handle)
{
    spi_handle = _spi_handle;

    lcd_reset();
    HAL_Delay(1);
    lcd_write_command(0x21); // extended commands
    lcd_write_command(0xB0); // vop/contrast
    lcd_write_command(0x04); // temp control
    lcd_write_command(0x14); // bias mode 1:40
    lcd_write_command(0x20); // basic commands
    lcd_write_command(0x0C); // normal mode
}

void lcd_clear()
{

    lcd_write_command(0b10000000);
    lcd_write_command(0b01000000);

    for (int i = 0; i < 504; i++)
        lcd_write_byte(0b00000000);
}

void lcd_draw(uint8_t *data, uint8_t size)
{
    for (int i = 0; i < size; i++)
    {
        lcd_write_byte(data[i]);
    }
}

void lcd_backlight(uint8_t enabled)
{
    HAL_GPIO_WritePin(LCD_LIGHT_PORT, LCD_LIGHT_PIN, enabled ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

static void lcd_write_command(uint8_t data)
{
    HAL_GPIO_WritePin(LCD_DATA_COMMAND_PORT, LCD_DATA_COMMAND_PIN, LCD_COMMAND_MODE);
    HAL_GPIO_WritePin(LCD_CHIP_ENABLE_PORT, LCD_CHIP_ENABLE_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(spi_handle, &data, 1, LCD_SPI_TIMEOUT);
    HAL_GPIO_WritePin(LCD_CHIP_ENABLE_PORT, LCD_CHIP_ENABLE_PIN, GPIO_PIN_SET);
}

static void lcd_write_data(uint8_t *data, uint8_t size)
{
    HAL_GPIO_WritePin(LCD_DATA_COMMAND_PORT, LCD_DATA_COMMAND_PIN, LCD_DATA_MODE);
    HAL_GPIO_WritePin(LCD_CHIP_ENABLE_PORT, LCD_CHIP_ENABLE_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(spi_handle, data, size, LCD_SPI_TIMEOUT);
    HAL_GPIO_WritePin(LCD_CHIP_ENABLE_PORT, LCD_CHIP_ENABLE_PIN, GPIO_PIN_SET);
}

static void lcd_reset()
{

    HAL_GPIO_WritePin(LCD_RESET_PORT, LCD_RESET_PIN, GPIO_PIN_RESET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(LCD_RESET_PORT, LCD_RESET_PIN, GPIO_PIN_SET);
}

static void lcd_write_byte(uint8_t data)
{
    lcd_write_data(&data, 1);
}