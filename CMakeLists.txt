cmake_minimum_required(VERSION 3.20)

project(stm32f103-bare-metal-template LANGUAGES C ASM)


# =========================================================
# Флаги компилятора
# =========================================================
set(CPU_FLAGS "-mcpu=cortex-m3 -mthumb")

# Флаги зависят от типа сборки: Debug — без оптимизаций, Release — с -O2.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

set(BASE_C_FLAGS "-mcpu=cortex-m3 -mthumb -Wall -Wextra -ffreestanding -fno-common -DSTM32F103xB")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${BASE_C_FLAGS} -O0 -g3")
    message(STATUS "Сборка: Debug (O0, g3)")
else()
    set(CMAKE_C_FLAGS "${BASE_C_FLAGS} -O2 -g")
    message(STATUS "Сборка: Release (O2, g)")
endif()

# =========================================================
# Исходники
# =========================================================
set(SOURCES
    src/main.c
    src/startup.c
)

# =========================================================
# Исполняемый файл (ELF)
# =========================================================
add_executable(firmware.elf ${SOURCES})

# Заголовки
target_include_directories(firmware.elf PRIVATE
    ${CMAKE_SOURCE_DIR}/include/cmsis
    ${CMAKE_SOURCE_DIR}/include/stm32
)

# Скрипт линкера и опции линковки
target_link_options(firmware.elf PRIVATE
    -mcpu=cortex-m3
    -mthumb
    "-T${CMAKE_SOURCE_DIR}/linker/stm32f103c8.ld"
    -nostartfiles
    "-Wl,-Map=${CMAKE_BINARY_DIR}/firmware.map,--cref"
    -Wl,--wrap=malloc
    -Wl,--wrap=free
    -Wl,--wrap=calloc
    -Wl,--wrap=realloc
)

# =========================================================
# Постсборка: .bin, .hex, size
# =========================================================
add_custom_command(TARGET firmware.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:firmware.elf> firmware.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:firmware.elf> firmware.hex
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:firmware.elf>
    COMMENT "Generating firmware.bin and firmware.hex"
)