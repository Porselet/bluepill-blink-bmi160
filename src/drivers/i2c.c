#include "i2c.h"
#include "stm32f1xx.h"

/* =========================================================
 * I2C1 на голых регистрах STM32F103
 * =========================================================
 * Пины:   PB6 = SCL, PB7 = SDA
 * Режим:  AF open-drain, 50 MHz
 * Скорость: 100 кГц (стандартный режим)
 * APB1:   8 МГц (после сброса, HSI)
 * ========================================================= */

#define I2C_TIMEOUT 1000000UL

/* ---------------------------------------------------------
 * Ожидание флага с таймаутом
 * --------------------------------------------------------- */
static int wait_sr1_set(uint32_t mask)
{
    uint32_t t = I2C_TIMEOUT;
    while (!(I2C1->SR1 & mask))
        if (--t == 0)
            return I2C_ERR;
    return I2C_OK;
}

static int wait_sr1_clear(uint32_t mask)
{
    uint32_t t = I2C_TIMEOUT;
    while (I2C1->SR1 & mask)
        if (--t == 0)
            return I2C_ERR;
    return I2C_OK;
}

/* ---------------------------------------------------------
 * Сброс флага ADDR.
 * Сбрасывается ТОЛЬКО последовательностью: чтение SR1, потом SR2.
 * Если этого не сделать — SCL останется прижатым в 0, шина зависнет.
 * --------------------------------------------------------- */
static inline void clear_addr_flag(void)
{
    volatile uint32_t tmp;
    tmp = I2C1->SR1;
    tmp = I2C1->SR2;
    (void)tmp;
}

/* ---------------------------------------------------------
 * Инициализация
 * --------------------------------------------------------- */
void i2c_init(void)
{
    /* 1. Тактирование: GPIOB (APB2), AFIO (APB2), I2C1 (APB1) */
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    for (volatile uint32_t i = 0; i < 1000; i++)
    {
    }

    /* 2. PB6/PB7 — AF open-drain, 50 MHz */
    GPIOB->CRL &= ~((0xFUL << 24) | (0xFUL << 28));
    GPIOB->CRL |= ((0xFUL << 24) | (0xFUL << 28));

    /* 3. Программный сброс I2C1 — на случай зависшего состояния */
    I2C1->CR1 |= I2C_CR1_SWRST;
    I2C1->CR1 &= ~I2C_CR1_SWRST;

    /* 4. Частота APB1 = 8 МГц */
    I2C1->CR2 = 8;
    I2C1->CCR = 40; /* 100 кГц */
    I2C1->TRISE = 9;

    /* 5. Включить */
    I2C1->CR1 |= I2C_CR1_PE;
}

/* ---------------------------------------------------------
 * START. Ждём, пока шина освободится, генерируем START, ждём SB.
 * --------------------------------------------------------- */
int i2c_start(void)
{
    /* Ждём, пока BUSY = 0 (шина свободна). С таймаутом, иначе
     * залипнем при аварии на линии. */
    uint32_t t = I2C_TIMEOUT;
    while (I2C1->SR2 & I2C_SR2_BUSY)
        if (--t == 0)
            return I2C_ERR;

    I2C1->CR1 |= I2C_CR1_START;

    return wait_sr1_set(I2C_SR1_SB);
}

/* ---------------------------------------------------------
 * STOP
 * --------------------------------------------------------- */
void i2c_stop(void)
{
    I2C1->CR1 |= I2C_CR1_STOP;
}

/* ---------------------------------------------------------
 * Отправка байта. 0 = ACK, -1 = NACK или timeout.
 * --------------------------------------------------------- */
int i2c_write_byte(uint8_t b)
{
    if (wait_sr1_set(I2C_SR1_TXE) != I2C_OK)
        return I2C_ERR;

    I2C1->DR = b;

    if (wait_sr1_set(I2C_SR1_BTF) != I2C_OK)
        return I2C_ERR;

    if (I2C1->SR1 & I2C_SR1_AF)
    {
        I2C1->SR1 &= ~I2C_SR1_AF; /* сброс NACK */
        return I2C_ERR;
    }
    return I2C_OK;
}

/* ---------------------------------------------------------
 * Чтение байта. ack=1 → ACK, ack=0 → NACK.
 * --------------------------------------------------------- */
uint8_t i2c_read_byte(uint8_t ack)
{
    if (ack)
        I2C1->CR1 |= I2C_CR1_ACK;
    else
        I2C1->CR1 &= ~I2C_CR1_ACK;

    if (wait_sr1_set(I2C_SR1_RXNE) != I2C_OK)
        return 0;

    return (uint8_t)I2C1->DR;
}

/* ---------------------------------------------------------
 * Отправка адреса слейва с битом R/W.
 * addr — 7-битный (НЕ сдвинутый).
 * rw   — 0=W, 1=R.
 * Внутри сбрасывает ADDR и проверяет NACK.
 * --------------------------------------------------------- */
int i2c_write_addr(uint8_t dev_addr, uint8_t rw)
{
    uint8_t b = (uint8_t)((dev_addr << 1) | (rw & 1));

    if (i2c_write_byte(b) != I2C_OK)
    {
        /* NACK от слейва — адрес не принят */
        return I2C_ERR;
    }

    /* Сбрасываем ADDR: без этого SCL останется прижатым в 0 */
    clear_addr_flag();

    return I2C_OK;
}

/* ---------------------------------------------------------
 * Верхний уровень: запись одного байта в регистр.
 * START addr+W reg val STOP
 * --------------------------------------------------------- */
int i2c_write_reg(uint8_t dev_addr, uint8_t reg, uint8_t val)
{
    if (i2c_start() != I2C_OK)
        return I2C_ERR;
    if (i2c_write_addr(dev_addr, 0) != I2C_OK)
    {
        i2c_stop();
        return I2C_ERR;
    }
    if (i2c_write_byte(reg) != I2C_OK)
    {
        i2c_stop();
        return I2C_ERR;
    }
    if (i2c_write_byte(val) != I2C_OK)
    {
        i2c_stop();
        return I2C_ERR;
    }
    i2c_stop();
    return I2C_OK;
}

/* ---------------------------------------------------------
 * Верхний уровень: чтение одного байта из регистра.
 * START addr+W reg rSTART addr+R data NACK STOP
 * --------------------------------------------------------- */
int i2c_read_reg(uint8_t dev_addr, uint8_t reg, uint8_t *out)
{
    if (i2c_start() != I2C_OK)
        return I2C_ERR;
    if (i2c_write_addr(dev_addr, 0) != I2C_OK)
    {
        i2c_stop();
        return I2C_ERR;
    }
    if (i2c_write_byte(reg) != I2C_OK)
    {
        i2c_stop();
        return I2C_ERR;
    }

    if (i2c_start() != I2C_OK)
        return I2C_ERR; /* repeated START */
    if (i2c_write_addr(dev_addr, 1) != I2C_OK)
    {
        i2c_stop();
        return I2C_ERR;
    }

    *out = i2c_read_byte(0); /* NACK — это последний байт */
    i2c_stop();

    return I2C_OK;
}

/* ---------------------------------------------------------
 * Верхний уровень: burst-чтение.
 * START addr+W reg rSTART addr+R d0 ACK d1 ACK ... dN NACK STOP
 * --------------------------------------------------------- */
int i2c_read_regs(uint8_t dev_addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
    if (len == 0)
        return I2C_ERR;

    if (i2c_start() != I2C_OK)
        return I2C_ERR;
    if (i2c_write_addr(dev_addr, 0) != I2C_OK)
    {
        i2c_stop();
        return I2C_ERR;
    }
    if (i2c_write_byte(reg) != I2C_OK)
    {
        i2c_stop();
        return I2C_ERR;
    }

    if (i2c_start() != I2C_OK)
        return I2C_ERR;
    if (i2c_write_addr(dev_addr, 1) != I2C_OK)
    {
        i2c_stop();
        return I2C_ERR;
    }

    /* Читаем len-1 байт с ACK */
    for (uint16_t i = 0; i < len - 1; i++)
    {
        buf[i] = i2c_read_byte(1);
    }
    /* Последний — с NACK */
    buf[len - 1] = i2c_read_byte(0);

    i2c_stop();
    return I2C_OK;
}