#include <stdint.h>

/* =========================================================
 * Символы, которые определяет линкер (см. stm32f103c8.ld)
 * ========================================================= */
extern uint32_t _estack;
extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

/* =========================================================
 * Точки входа
 * ========================================================= */
int main(void);
void Reset_Handler(void);
void Default_Handler(void);

/* =========================================================
 * Векторная таблица.
 * Секция .isr_vector — её линкер положит в самое начало FLASH.
 * Первый элемент — начальное значение SP.
 * Второй — адрес Reset_Handler.
 * ========================================================= */
__attribute__((section(".isr_vector")))
const uint32_t vector_table[] = {
    (uint32_t)&_estack,         /* 0x08000000: initial SP */
    (uint32_t)&Reset_Handler,   /* 0x08000004: Reset */
    (uint32_t)&Default_Handler, /* 0x08000008: NMI */
    (uint32_t)&Default_Handler, /* 0x0800000C: HardFault */
    /* ... остальные мы добавим позже, когда дойдём до прерываний ... */
};

/* =========================================================
 * Reset_Handler: копируем .data, обнуляем .bss, вызываем main
 * ========================================================= */
void Reset_Handler(void)
{
    /* 1. Скопировать .data из FLASH (_sidata) в RAM (_sdata.._edata) */
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;
    while (dst < &_edata)
        *dst++ = *src++;

    /* 2. Обнулить .bss (_sbss.._ebss) */
    dst = &_sbss;
    while (dst < &_ebss)
        *dst++ = 0;

    /* 3. Перейти в main. Если main вернётся — зависнуть. */
    main();
    while (1)
    {
    }
}

/* =========================================================
 * Заглушка для всех неиспользуемых прерываний
 * ========================================================= */
void Default_Handler(void)
{
    while (1)
    {
    }
}