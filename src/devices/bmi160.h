#ifndef BMI160_H
#define BMI160_H

#include <stdint.h>

/* =========================================================
 * Драйвер BMI160 (IMU: акселерометр + гироскоп)
 * =========================================================
 * Адрес I2C зависит от пина SA0:
 *   0x68 — SA0 притянут к GND
 *   0x69 — SA0 подтянут к VCC или висит в воздухе (внутренний pull-up)
 *
 * bmi160_init() сам определяет адрес и запоминает его.
 * Все остальные функции работают с уже найденным адресом.
 * ========================================================= */

/* Возможные I2C-адреса */
#define BMI160_ADDR_SA0_LOW 0x68
#define BMI160_ADDR_SA0_HIGH 0x69

/* Регистры BMI160 (только нужные нам сейчас) */
#define BMI160_REG_CHIP_ID 0x00 /* всегда 0xD1 */
#define BMI160_REG_ERR 0x02
#define BMI160_REG_PMU_STATUS 0x03
#define BMI160_REG_DATA 0x0C /* начало блока данных: gyro X/Y/Z, acc X/Y/Z */
#define BMI160_REG_CMD 0x7E

/* Ожидаемое значение CHIP_ID */
#define BMI160_CHIP_ID_VALUE 0xD1

/* Коды возврата (совпадают с i2c.h) */
#define BMI160_OK 0
#define BMI160_ERR -1

/* Инициализация: скан адресов, проверка CHIP_ID.
 * Возвращает 0, если чип найден; -1 при ошибке. */
int bmi160_init(void);

/* Адрес, с которым работаем (определяется в bmi160_init) */
uint8_t bmi160_get_addr(void);

/* Чтение/запись одного регистра */
int bmi160_read_reg(uint8_t reg, uint8_t *out);
int bmi160_write_reg(uint8_t reg, uint8_t val);

/* Burst-чтение нескольких регистров подряд */
int bmi160_read_regs(uint8_t reg, uint8_t *buf, uint16_t len);

#endif