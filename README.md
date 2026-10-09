# STM32F103 Bare-Metal Template

Минимальный шаблон проекта для STM32F103C8T6 (Blue Pill) **без HAL** —
на голых регистрах, с самописным startup-кодом и linker script.

Собран на **CMake + Ninja + arm-none-eabi-gcc**, отладка через
**OpenOCD + ST-Link V2** и **Cortex-Debug** в VSCode.

---

## Что включено

- **CMSIS-заголовки** для STM32F1 (`stm32f1xx.h`, `stm32f103xb.h`, `core_cm3.h`).
- **Свой linker script** — карта памяти и секции расписаны вручную (`linker/stm32f103c8.ld`).
- **Свой startup** — векторная таблица, `Reset_Handler`, обнуление `.bss`, копирование `.data` (`src/startup.c`).
- **Никакого HAL.** Всё через регистры через CMSIS-структуры.
- **Запрет кучи** — `malloc`/`free`/`calloc`/`realloc` блокируются на уровне линкера.
- **Задел под отладку** — `.vscode/launch.json` для Cortex-Debug.

---

## Структура

```
.
├── CMakeLists.txt              # рецепт сборки
├── cmake/
│   └── arm-none-eabi.cmake     # тулчейн-файл (компилятор, архитектура)
├── include/
│   ├── cmsis/                  # CMSIS core-заголовки
│   └── stm32/                  # device-заголовки для STM32F1
├── linker/
│   └── stm32f103c8.ld          # скрипт линкера (карта памяти)
├── src/
│   ├── main.c                  # твой код
│   └── startup.c               # векторная таблица + Reset_Handler
└── .vscode/
    ├── launch.json             # конфиг отладки
    ├── tasks.json              # задачи (Build, Flash)
    └── c_cpp_properties.json   # конфиг IntelliSense
```

---

## Требования

- **arm-none-eabi-gcc** — компилятор (скачать: developer.arm.com/downloads/-/gnu-rm)
- **CMake** ≥ 3.20 — cmake.org
- **Ninja** — github.com/ninja-build/ninja/releases
- **OpenOCD** — для прошивки/отладки (github.com/openocd-org/openocd)
- **ST-Link V2** — программатор для Blue Pill
- **VSCode** + расширения: **C/C++**, **CMake Tools**, **Cortex-Debug**

---

## Сборка

```bash
cmake -G Ninja -B build "-DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake" "-DCMAKE_BUILD_TYPE=Debug"
cmake --build build
```

После сборки в `build/` появятся:
- `firmware.elf` — готовая программа с отладочной информацией.
- `firmware.bin` — «сырые байты» для прошивки.
- `firmware.hex` — Intel HEX.
- `firmware.map` — карта памяти (что куда легло).

Типы сборки:
- **Debug** — `-O0 -g3`, для отладки. Переменные не оптимизируются, всё видно.
- **Release** — `-O2 -g`, для прошивки.

Переключение:
```bash
cmake -B build "-DCMAKE_BUILD_TYPE=Release"
cmake --build build
```

---

## Прошивка

Через OpenOCD из командной строки (замени путь к OpenOCD на свой):

```bash
openocd -s C:/EmbeddedTools/openocd/openocd/scripts \
  -f interface/stlink.cfg -f target/stm32f1x.cfg \
  -c "program build/firmware.elf verify reset exit"
```

Или через VSCode: **Ctrl+Shift+P** → **Tasks: Run Task** → **Flash**.

---

## Отладка

В VSCode: **F5**. Отладчик стартует OpenOCD в фоне, подключится, остановится
на `Reset_Handler` (или на `main`, как настроишь в `launch.json`).

Точки останова, просмотр регистров, стека, памяти — всё работает.

---

## Память чипа

- **Flash:** 64 КБ (`0x08000000 .. 0x08010000`).
- **RAM:** 20 КБ (`0x20000000 .. 0x20005000`).

Если у тебя ревизия с 128 КБ (часто попадается у китайских C8), поправь
`LENGTH = 128K` в `linker/stm32f103c8.ld`.

---

## Что делать дальше

1. Открой `src/main.c`, замени содержимое `main` на свой код.
2. Если добавляешь новые `.c` — допиши их в `set(SOURCES ...)` в `CMakeLists.txt`.
3. Для работы с новыми периферийными блоками используй CMSIS-структуры (`RCC`, `GPIOA`, `USART1` и т.д.).

---

## Лицензия

MIT — делай что хочешь.