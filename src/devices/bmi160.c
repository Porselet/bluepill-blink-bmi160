#include "bmi160.h"
#include "drivers/i2c.h"

/* =========================================================
 * Текущий I2C-адрес BMI160.
 * Определяется в bmi160_init() сканированием.
 * ========================================================= */
static uint8_t bmi160_addr = 0;

/* ---------------------------------------------------------
 * Попытка прочитать CHIP_ID по заданному адресу.
 * Возвращает 0, если чип ответил и ID совпал.
 * --------------------------------------------------------- */
static int bmi160_probe(uint8_t addr)
{
    uint8_t id = 0;

    if (i2c_read_reg(addr, BMI160_REG_CHIP_ID, &id) != I2C_OK)
        return BMI160_ERR;

    if (id != BMI160_CHIP_ID_VALUE)
        return BMI160_ERR;

    return BMI160_OK;
}

/* ---------------------------------------------------------
 * Инициализация: ищем чип на обоих возможных адресах.
 * --------------------------------------------------------- */
int bmi160_init(void)
{
    /* Сначала пробуем 0x69 (SA0 в воздухе / подтянут) */
    if (bmi160_probe(BMI160_ADDR_SA0_HIGH) == BMI160_OK)
    {
        bmi160_addr = BMI160_ADDR_SA0_HIGH;
        return BMI160_OK;
    }

    /* Потом 0x68 (SA0 притянут к GND) */
    if (bmi160_probe(BMI160_ADDR_SA0_LOW) == BMI160_OK)
    {
        bmi160_addr = BMI160_ADDR_SA0_LOW;
        return BMI160_OK;
    }

    /* Ни один не ответил */
    return BMI160_ERR;
}

uint8_t bmi160_get_addr(void)
{
    return bmi160_addr;
}

/* ---------------------------------------------------------
 * Обёртки над i2c_* с подстановкой bmi160_addr
 * --------------------------------------------------------- */
int bmi160_read_reg(uint8_t reg, uint8_t *out)
{
    return i2c_read_reg(bmi160_addr, reg, out);
}

int bmi160_write_reg(uint8_t reg, uint8_t val)
{
    return i2c_write_reg(bmi160_addr, reg, val);
}

int bmi160_read_regs(uint8_t reg, uint8_t *buf, uint16_t len)
{
    return i2c_read_regs(bmi160_addr, reg, buf, len);
}