#include "stm32f1xx_hal.h"
#include "display.h"
#include "string.h"

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

#define WIDTH 84
#define HEIGHT 48
#define BUFFER_SIZE (WIDTH * HEIGHT / 8)

uint8_t font[FONT_LENGTH][LETTER_LENGTH];
SPI_HandleTypeDef *spi_handle;

static void lcd_reset(void);
static void lcd_write_command(uint8_t data);
static void lcd_write_data(uint8_t *data, uint16_t size);
static void lcd_write_byte(uint8_t data);
static void lcd_set_pixel(uint8_t x, uint8_t y, uint8_t val);
static void lcd_set_cursor_y(uint8_t y);
static void lcd_set_cursor_x(uint8_t x);
static void lcd_set_cursor(uint8_t x, uint8_t y);
static void lcd_set_extended_commands();
static void lcd_set_basic_commands();
static void lcd_set_normal_mode();

static uint8_t display_buffer[BUFFER_SIZE] = {0};

void lcd_init(SPI_HandleTypeDef *_spi_handle)
{
    spi_handle = _spi_handle;

    lcd_reset();
    HAL_Delay(1);
    lcd_set_normal_mode();
}

void lcd_set_normal_mode()
{
    lcd_write_command(0x0C);
}

void lcd_set_contrast(uint8_t contrast)
{
    lcd_set_extended_commands();
    lcd_write_command(0x80 | contrast);
    lcd_set_basic_commands();
}

void lcd_set_temp_control(uint8_t temp_control)
{
    lcd_set_extended_commands();
    lcd_write_command(0x04 | temp_control);
    lcd_set_basic_commands();
}

void lcd_set_bias_mode(uint8_t bias_mode)
{
    lcd_set_extended_commands();
    lcd_write_command(0x10 | bias_mode);
    lcd_set_basic_commands();
}

void lcd_clear()
{
    lcd_set_cursor(0, 0);

    for (int i = 0; i < BUFFER_SIZE; i++)
        display_buffer[i] = 0;
}

void lcd_set_font(uint8_t _font[FONT_LENGTH][LETTER_LENGTH])
{
    for (int i = 0; i < FONT_LENGTH; i++)
        for (int j = 0; j < LETTER_LENGTH; j++)
            font[i][j] = _font[i][j];
}

void lcd_draw_bytemap(uint8_t *bytemap, uint8_t width, uint8_t height, uint8_t x, uint8_t y)
{
    for (int current_y = 0; current_y < height; current_y++)
    {
        for (int current_x = 0; current_x < width; current_x++)
        {
            int current_bytemap_index = current_y * width + current_x;
            lcd_set_pixel(x + current_x, y + current_y, bytemap[current_bytemap_index]);
        }
    }
}

void lcd_draw_bitmap(uint8_t *bitmap, uint8_t width, uint8_t height, uint8_t target_x, uint8_t target_y)
{
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            lcd_set_pixel(target_x + x, target_y + y, bitmap[y] & (1 << x));
}

void lcd_print(char *text, uint8_t x, uint8_t y)
{
    for (int i = 0; i < strlen(text); i++)
    {
        lcd_draw_bitmap(&font[text[i]], 8, 8, x + i * 8, y);
    }
}

void lcd_update()
{
    /*
    For some fucking unknown reasons the line below
    does not work correctly in this case without delay.
    It sends only first couple of bytes. WIth delay everything works ok:

    uint8_t bitmap[84];
    for (int i = 0; i < 84; i++) bitmap[i] = 255;
    // HAL_Delay(1);
    lcd_draw(bitmap, 84);
    */

    for (int i = 0; i < BUFFER_SIZE; i++)
        lcd_write_byte(display_buffer[i]);
}

void lcd_backlight(uint8_t enabled)
{
    HAL_GPIO_WritePin(LCD_LIGHT_PORT, LCD_LIGHT_PIN, enabled ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

static void lcd_set_pixel(uint8_t x, uint8_t y, uint8_t val)
{
    int y_offset = y / 8 * WIDTH;

    if (val)
        display_buffer[y_offset + x] |= 1 << y % 8;
    else
        display_buffer[y_offset + x] &= ~(1 << y % 8);
}

static void lcd_set_cursor_y(uint8_t y)
{
    lcd_write_command(0b01000000 | y);
}

static void lcd_set_cursor_x(uint8_t x)
{
    lcd_write_command(0b10000000 | x);
}

static void lcd_set_cursor(uint8_t x, uint8_t y)
{
    lcd_set_cursor_x(x);
    lcd_set_cursor_y(y);
}

static void lcd_write_byte(uint8_t data)
{
    lcd_write_data(&data, 1);
}

static void lcd_write_multiple_bytes(uint8_t *data, uint16_t size)
{
    lcd_write_data(data, size);
}

static void lcd_set_extended_commands()
{
    lcd_write_command(0x21);
}

static void lcd_set_basic_commands()
{
    lcd_write_command(0x20); // basic commands
}

static void lcd_write_command(uint8_t data)
{
    HAL_GPIO_WritePin(LCD_DATA_COMMAND_PORT, LCD_DATA_COMMAND_PIN, LCD_COMMAND_MODE);
    HAL_GPIO_WritePin(LCD_CHIP_ENABLE_PORT, LCD_CHIP_ENABLE_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(spi_handle, &data, 1, LCD_SPI_TIMEOUT);
    HAL_GPIO_WritePin(LCD_CHIP_ENABLE_PORT, LCD_CHIP_ENABLE_PIN, GPIO_PIN_SET);
}

static void lcd_write_data(uint8_t *data, uint16_t size)
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
