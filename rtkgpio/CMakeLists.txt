cmake_minimum_required(VERSION 3.17)

#set(CMAKE_INSTALL_PREFIX "C:/workspace/dev")

set(PRJNAME lib${SIMPLENAME})
project(${PRJNAME} VERSION 1.0.0 LANGUAGES C)
set(CMAKE_C_STANDARD 99)

# -extra-arg=-Wno-unknown-warning-option;
set(CMAKE_CXX_CLANG_TIDY
        clang-tidy;
        -header-filter=.;
        -checks=*,-fuchsia-default-arguments-calls,-fuchsia-trailing-return,-google-runtime-references;
        )

# https://rix0r.nl/blog/2015/08/13/cmake-guide/
# e.g. to install libs into correct locations on all platforms
include(GNUInstallDirs)

set(NAME rtkgpio)
add_library(${NAME} SHARED)

if (MSVC)
    # warning level 4 and all warnings as errors
    target_compile_options(${NAME} PRIVATE /W4 /WX)
else ()
    # lots of warnings and all warnings as errors
    target_compile_options(${NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
    target_compile_options(${NAME} PRIVATE -Wshadow)
    target_compile_options(${NAME} PRIVATE -Wdouble-promotion)
    target_compile_options(${NAME} PRIVATE -Wformat=2 -Wformat-overflow -Wformat-truncation)
    target_compile_options(${NAME} PRIVATE -Wundef)
    target_compile_options(${NAME} PRIVATE -fno-common)
    target_compile_options(${NAME} PRIVATE -Wconversion)
    target_compile_options(${NAME} PRIVATE -Werror)
endif ()

target_sources(${NAME} PRIVATE
        src/rtkgpio.c
        include/rtkgpio/rtkgpio.h
        include/rtkgpio/rtkgpio_dll.h
        )

target_compile_definitions(${NAME} PRIVATE RTKGPIO_BUILDING_DLL=1)

target_include_directories(${NAME}
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        )

target_link_libraries(${NAME} PRIVATE serialport)

# BEG: print all variables
#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()
# END: print all variables

# 'make install' to the correct locations (provided by GNUInstallDirs).
install(TARGETS ${NAME} EXPORT ${NAME}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # This is for Windows
        )
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# This makes the project importable from the install directory
# Put config file in per-project dir (name MUST match), can also
# just go into 'cmake'.
install(EXPORT ${NAME} DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${NAME})

# This makes the project importable from the build directory
export(TARGETS ${NAME} FILE ${NAME}.cmake)
