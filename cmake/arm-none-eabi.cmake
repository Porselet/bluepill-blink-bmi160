# Тулчейн для arm-none-eabi (bare-metal, без ОС)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Ищем компилятор по имени — CMake сам найдёт его в PATH
set(TOOLCHAIN_PREFIX arm-none-eabi-)

set(CMAKE_C_COMPILER   ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_OBJCOPY      ${TOOLCHAIN_PREFIX}objcopy)
set(CMAKE_SIZE         ${TOOLCHAIN_PREFIX}size)

# Не пытаться линковать тестовую программу при проверке компилятора —
# для bare-metal это ломается, поэтому просто пропускаем.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Ищем библиотеки/заголовки только внутри тулчейна, а не в системе хоста
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)