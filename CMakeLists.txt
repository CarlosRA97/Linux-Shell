cmake_minimum_required(VERSION 3.7.2)
project(Shell C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ./bin)

add_library( pipe OBJECT 
    pipe/pipe.h 
    pipe/pipe.c
)
add_library( job_control OBJECT 
    job/job_control.h 
    job/job_control.c
)
add_library( readline OBJECT 
    readline/readline.h 
    readline/readline.c 
    readline/cursor.h
    readline/cursor.c
)
add_library( internal_commands OBJECT
    internal_commands/internal_commands.h
    internal_commands/bg.c
    internal_commands/fg.c
    internal_commands/cd.c
    internal_commands/historial.c
    internal_commands/common.c
)
add_library( history OBJECT
    history/history.h 
    history/history.c 
)
add_library( common OBJECT
    common/common.h
    common/common.c
)

add_executable(
    Shell 
    Shell.c 
    $<TARGET_OBJECTS:job_control>
    $<TARGET_OBJECTS:history>
    $<TARGET_OBJECTS:pipe>
    $<TARGET_OBJECTS:readline>
    $<TARGET_OBJECTS:internal_commands>
    $<TARGET_OBJECTS:common>
)

enable_testing()
add_executable(tester test/tester.c)
add_test(Tester bin/tester)

add_executable(find_pipe_test test/find_pipe_test.c $<TARGET_OBJECTS:pipe>)
add_test(FindPipeWithPipeInArgument bin/find_pipe_test "ls" "|" "sort")
add_test(FindPipeWithoutPipeInArgument bin/find_pipe_test "ls" "-la")

add_executable(find_pipes_test test/find_pipes_test.c $<TARGET_OBJECTS:pipe>)
add_test(FindPipes bin/find_pipes_test "ls" "|" "sort" "|" "wc")

add_executable(new_command_test test/new_command_test.c $<TARGET_OBJECTS:history>)
add_test(NewCommandWithPipeAndArguments bin/new_command_test "ls" "-la" "|" "sort")
