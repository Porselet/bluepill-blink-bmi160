/* =========================================================
 * Linker script для STM32F103C8 (Blue Pill)
 * Flash: 64 KB @ 0x08000000
 * RAM:   20 KB @ 0x20000000
 * ========================================================= */

ENTRY(Reset_Handler)

/* ---------- MEMORY: карта памяти ---------- */
MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
    RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}

/* ---------- Символ вершины стека ---------- */
_estack = ORIGIN(RAM) + LENGTH(RAM);   /* 0x20005000 — вершина RAM */

/* ---------- Размеры кучи и стека (нам пока не критично) ---------- */

_Min_Stack_Size = 0x400;   /* 1 КБ */

/* ---------- SECTIONS: раскладка ---------- */
SECTIONS
{
    /* ===== 1. Векторная таблица + код + константы ===== */
    .text :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))   /* векторная таблица — обязательно в самом начале */
        *(.text)               /* код */
        *(.text*)
        *(.rodata)             /* константы */
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
    } > FLASH

    /* ===== 2. Инициализированные данные ===== */
    /* Хранится во FLASH, при старте копируется в RAM.
       _sidata — где во FLASH лежит образ;
       _sdata.._edata — куда копировать в RAM. */
    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > RAM AT> FLASH

    _sidata = LOADADDR(.data);

    /* ===== 3. BSS: неинициализированные данные ===== */
    /* Живёт в RAM, при старте обнуляется.
       _sbss.._ebss — диапазон для обнуления. */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > RAM

    /* ===== 4. Проверка, что стек влезают ===== */
    ._user_stack :
    {
        . = ALIGN(8);
        PROVIDE ( end = . );
        PROVIDE ( _end = . );
        . = . + _Min_Stack_Size;
        . = ALIGN(8);
    } > RAM

    /* ===== 5. Отладочная информация (не грузится в МК) ===== */
    /DISCARD/ :
    {
        *(.note*)
        *(.comment*)
    }
}