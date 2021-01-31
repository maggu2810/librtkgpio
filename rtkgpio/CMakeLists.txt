cmake_minimum_required(VERSION 3.17)

set(NAME rtkgpio)
set(PRJNAME lib${SIMPLENAME})

project(${PRJNAME} VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)

# -extra-arg=-Wno-unknown-warning-option;
set(CMAKE_CXX_CLANG_TIDY
        clang-tidy;
        -header-filter=.;
        -checks=*,-fuchsia-default-arguments-calls,-fuchsia-trailing-return,-google-runtime-references;
        )

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
else ()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
    add_compile_options(-Wshadow)
    add_compile_options(-Wdouble-promotion)
    add_compile_options(-Wformat=2 -Wformat-overflow -Wformat-truncation)
    add_compile_options(-Wundef)
    add_compile_options(-fno-common)
    add_compile_options(-Wconversion)
endif ()

# https://rix0r.nl/blog/2015/08/13/cmake-guide/
# e.g. to install libs into correct locations on all platforms
include(GNUInstallDirs)

# all source files
file(GLOB src
        ./src/*.c
        )

# all header files
file(GLOB hdr
        ./include/${NAME}/*.h
        )

add_library(${NAME} SHARED ${src} ${hdr})

if (WIN32)
    if (DEFINED SERIALPORT_INCLUDE_DIRS)
        include_directories(${SERIALPORT_INCLUDE_DIRS})
    endif (DEFINED SERIALPORT_INCLUDE_DIRS)
    if (DEFINED SERIALPORT_LIBRARIES)
        target_link_libraries(${NAME} ${SERIALPORT_LIBRARIES})
    endif (DEFINED SERIALPORT_LIBRARIES)
    #find_library(SERIALPORT_LIBRARIES NAMES serialport PATHS c:/Users/de23a4/workspace/eclipse-cdt/externals/lib)
    #include_directories(c:/Users/de23a4/workspace/eclipse-cdt/externals/include)
    #target_link_libraries(${NAME} c:/Users/de23a4/workspace/eclipse-cdt/externals/lib/libserialport.dll)
endif (WIN32)

if (UNIX)
    #find_package(serialport REQUIRED)
    #include_directories(${SERIALPORT_INCLUDE_DIRS})
    #target_link_libraries(${NAME} ${SERIALPORT_LIBRARIES})
    target_link_libraries(${NAME} serialport)
endif (UNIX)

target_include_directories(${NAME}
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        )

#target_compile_options(${NAME} PRIVATE -Werror)

# 'make install' to the correct locations (provided by GNUInstallDirs).
install(TARGETS ${NAME} EXPORT ${NAME}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})  # This is for Windows
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# This makes the project importable from the install directory
# Put config file in per-project dir (name MUST match), can also
# just go into 'cmake'.
install(EXPORT ${NAME} DESTINATION share/${NAME}/cmake)

# This makes the project importable from the build directory
export(TARGETS ${NAME} FILE ${NAME}.cmake)
