cmake_minimum_required(VERSION 3.7.2)
project(Shell C)

set(CMAKE_C_STANDARD 99)

add_executable(
    Shell 
    Shell_project.c 
    job/job_control.h 
    job/job_control.c 
    history/history.h 
    history/history.c 
    pipe/pipe.h 
    pipe/pipe.c 
    readline/readline.h 
    readline/readline.c 
    readline/cursor.h
    readline/cursor.c 
)
