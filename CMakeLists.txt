# CMakeList.txt: файл проекта CMake верхнего уровня; проведите глобальную конфигурацию
# и включите сюда подпроекты.
#
project ("Server-Client-Application")
cmake_minimum_required (VERSION 3.13)
find_package (Threads)
# Включите подпроекты.
include_directories("Common")
add_subdirectory("Common")
add_subdirectory("Server")
add_subdirectory("Client")
target_link_libraries (Server ${CMAKE_THREAD_LIBS_INIT})
