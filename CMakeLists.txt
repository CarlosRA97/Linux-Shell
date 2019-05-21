cmake_minimum_required(VERSION 3.14)
project(Shell C)

set(CMAKE_C_STANDARD 99)

add_executable(Shell Shell_project.c job_control.c job_control.h)
