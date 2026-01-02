#
# CMakeLists.txt
#
#  Created on: 02 jan. 2026
#      Author: Ludo
#

# Minimum CMake version.
cmake_minimum_required(VERSION 3.23)

# Project creation.
project(trcs-driver)

# Build mode.
if(NOT DEFINED BUILD_MODE)
    set(BUILD_MODE "STATIC")
endif()

# Create library.
add_library(${PROJECT_NAME} ${BUILD_MODE})

# Static library.
if(${BUILD_MODE} STREQUAL STATIC)

    # Macro to convert options to variables.
    macro(add_compilation_flag FLAG_NAME FLAG_DESCRIPTION FLAG_DEFAULT_VALUE)
        option(FLAG_NAME FLAG_DESCRIPTION FLAG_DEFAULT_VALUE)
        set(${FLAG_NAME} ${FLAG_DEFAULT_VALUE} CACHE STRING ${FLAG_DESCRIPTION})
        list(APPEND COMPILATION_FLAGS_LIST ${FLAG_NAME})
    endmacro()
    
    # Compilation flags.
    add_compilation_flag(TRCS_DRIVER_DISABLE "Disable the TRCS driver." OFF)
    add_compilation_flag(TRCS_DRIVER_GPIO_ERROR_BASE_LAST "Last error base of the low level GPIO driver." 0)
    add_compilation_flag(TRCS_DRIVER_TIMER_ERROR_BASE_LAST "Last error base of the low level timer driver." 0)
    add_compilation_flag(TRCS_DRIVER_ADC_ERROR_BASE_LAST "Last error base of the low level ADC driver." 0)
    add_compilation_flag(TRCS_DRIVER_ADC_RANGE_MV "Conversion range of the low level ADC peripheral in mV." 3300)
    
    # Remove OFF flags from list and keep flags set to value 0.
    foreach(FLAG ${COMPILATION_FLAGS_LIST})
        if((NOT ${FLAG} STREQUAL OFF) AND (NOT ${FLAG} STREQUAL ON))
            set(${FLAG} "(${${FLAG}})")
        endif()
    endforeach()   
    
    # Create flags file.
    configure_file(trcs_driver_flags.h.in trcs_driver_flags.h @ONLY)
    
    # Fixed compilation flags of dependencies.
    if(${BUILD_MODE} STREQUAL STATIC)
        target_compile_definitions(${PROJECT_NAME}
            PRIVATE
                EMBEDDED_UTILS_DISABLE_FLAGS_FILE
        )
    endif()
    
    # Dependencies folders.
    target_include_directories(${PROJECT_NAME}
        PUBLIC
            ${CMAKE_CURRENT_BINARY_DIR}
            ${TYPES_PATH}
            ${EMBEDDED_UTILS_PATH}/inc
    )
    
    # Print archive size.
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
        COMMAND ${CMAKE_SIZE_UTIL} -t lib${PROJECT_NAME}.a
    )
    
endif()

# Source files list.
target_sources(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/trcs_hw.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/trcs.c
)

# Header files folder.
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)