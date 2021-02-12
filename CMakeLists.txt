# CMakeList.txt: файл проекта CMake верхнего уровня; проведите глобальную конфигурацию
# и включите сюда подпроекты.
#
cmake_minimum_required (VERSION 3.8)

project ("Server-Client-Application")

# Включите подпроекты.
include_directories("Common")
add_subdirectory("Common")
add_subdirectory("Server")
add_subdirectory("Client")
