cmake_minimum_required(VERSION 3.10)
project(proc_pipes C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# parent и child кладём в одну папку: parent ищет child рядом с собой
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(MSVC)
    add_compile_options(/W4)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra)
endif()

# --- common: ошибки и чтение строк (платформонезависимо) ---
add_library(common STATIC common/errors.c common/io_utils.c)
target_include_directories(common PUBLIC common)

# --- os: реализация выбирается по платформе ---
if(WIN32)
    set(OS_SRC os/os_win.c)
else()
    set(OS_SRC os/os_linux.c)
endif()
add_library(os STATIC ${OS_SRC})
target_include_directories(os PUBLIC os)
target_link_libraries(os PUBLIC common)

# --- две отдельные программы ---
add_executable(child child/main.c)
target_link_libraries(child PRIVATE common)

add_executable(parent parent/main.c)
target_link_libraries(parent PRIVATE os common)
add_dependencies(parent child)

# --- cmake --build build --target run : собрать и запустить parent ---
add_custom_target(run
    COMMAND parent
    DEPENDS parent child
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    USES_TERMINAL)

# --- ctest: автоматическая проверка ---
enable_testing()
add_test(NAME reverse_and_split
    COMMAND ${CMAKE_COMMAND}
            -DPARENT=$<TARGET_FILE:parent>
            -DWORK=${CMAKE_BINARY_DIR}/test_work
            -P ${CMAKE_SOURCE_DIR}/tests/run_test.cmake)
