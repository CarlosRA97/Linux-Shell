cmake_minimum_required(VERSION 3.7.2)
project(Shell C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ./bin)

add_executable(
    Shell 
    Shell.c 
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
    internal_commands/internal_commands.h
    internal_commands/bg.c
    internal_commands/fg.c
    internal_commands/cd.c
    internal_commands/historial.c
    internal_commands/common.c
    common/common.h
    common/common.c
)
