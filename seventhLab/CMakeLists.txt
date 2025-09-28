cmake_minimum_required(VERSION 3.10)
project(DependencyInjection)

# Установка стандарта C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Добавление исполняемого файла
add_executable(dependency_injection main.cpp)

# Настройки компилятора
if(MSVC)
    target_compile_options(dependency_injection PRIVATE /W4)
else()
    target_compile_options(dependency_injection PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Настройки для отладки
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
