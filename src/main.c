#include "stm32f1xx.h"
#include "drivers/i2c.h"
#include "devices/bmi160.h"

int main(void)
{
    /* 1. Инициализируем I2C1 (PB6=SCL, PB7=SDA) */
    i2c_init();

    /* 2. Ищем BMI160 на шине. Функция сама пробует 0x69 и 0x68. */
    int status = bmi160_init();

    /* 3. Точка останова здесь.
     *    Смотрим в WATCH:
     *      status            → 0 = чип найден
     *      bmi160_get_addr() → рабочий адрес (0x68 или 0x69)
     *
     *    Хочешь узнать CHIP_ID отдельно — он уже прочитан внутри
     *    bmi160_probe(); смотри результат через статус bmi160_init.
     *    Для явного чтения ID после init:
     *      uint8_t id; bmi160_read_reg(BMI160_REG_CHIP_ID, &id);
     *    и добавь id в WATCH. */

    uint8_t id = 0;
    bmi160_read_reg(BMI160_REG_CHIP_ID, &id);

    while (1)
    {
    }
}