cmake_minimum_required(VERSION 3.22)

# --- 1. FIX: ЗАБОРОНЯЄМО ПЕРЕВІРКУ КОМПІЛЯТОРА (лікує помилку broken compiler) ---
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME lb3_Receiver)

# Core project settings - ВМИКАЄМО C++ (CXX)
project(${CMAKE_PROJECT_NAME} C CXX ASM)
message("Build type: " ${CMAKE_BUILD_TYPE})

# --- 2. FIX: ГЛОБАЛЬНО ВМИКАЄМО THUMB MODE (лікує помилку isb 0xF) ---
add_compile_options(-mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard)
add_link_options(-mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -lc -lm -lnosys)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# --- 3. АВТОПОШУК ТВОЇХ ФАЙЛІВ (як в Eclipse) ---
file(GLOB_RECURSE USER_SOURCES "Core/Src/*.c" "Core/Src/*.cpp")

# ВАЖЛИВО: Видаляємо system_stm32f4xx.c зі списку, бо CubeMX його вже додав вище
list(FILTER USER_SOURCES EXCLUDE REGEX "system_stm32f4xx.c")

# Додаємо знайдені файли до проекту
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    ${USER_SOURCES}
)

# Підключаємо заголовки
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    Core/Inc
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
)