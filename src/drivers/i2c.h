#ifndef I2C_H
#define I2C_H

#include <stdint.h>

/* =========================================================
 * Драйвер I2C1 (PB6=SCL, PB7=SDA) на голых регистрах STM32F103.
 *
 * API:
 *   - низкий уровень: start / stop / write_byte / read_byte
 *   - адресация: write_addr / read_addr (шлют addr+RW и сбрасывают ADDR)
 *   - верхний уровень: write_reg / read_reg / read_regs (транзакции целиком)
 *
 * Возврат: 0 = успех, -1 = ошибка (NACK или таймаут).
 * ========================================================= */

/* Возвраты */
#define I2C_OK 0
#define I2C_ERR -1

/* --- низкий уровень --- */
void i2c_init(void);
int i2c_start(void);
void i2c_stop(void);
int i2c_write_byte(uint8_t b);      /* 0 = ok, -1 = NACK/timeout */
uint8_t i2c_read_byte(uint8_t ack); /* ack=1 → шлём ACK, ack=0 → NACK */

/* --- адресация ---
 * rw = 0 → запись, rw = 1 → чтение.
 * Внутри делает сброс флага ADDR (чтение SR1+SR2). */
int i2c_write_addr(uint8_t dev_addr, uint8_t rw);

/* --- верхний уровень: транзакции целиком --- */

/* Записать один байт в регистр:  START addr+W reg data STOP */
int i2c_write_reg(uint8_t dev_addr, uint8_t reg, uint8_t val);

/* Прочитать один байт:  START addr+W reg rSTART addr+R data NACK STOP */
int i2c_read_reg(uint8_t dev_addr, uint8_t reg, uint8_t *out);

/* Прочитать несколько байт подряд (burst-read):
 * после рег-адреса слейв сам инкрементирует адрес регистра. */
int i2c_read_regs(uint8_t dev_addr, uint8_t reg, uint8_t *buf, uint16_t len);

#endif