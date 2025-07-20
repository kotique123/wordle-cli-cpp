cmake_minimum_required(VERSION 3.16)

project(wordle-cli VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build options
option(BUILD_TESTS "Build test executable" OFF)
option(BUILD_RELEASE "Build in release mode" ON)

# Platform detection
if(WIN32)
    set(PLATFORM "Windows")
    set(PLATFORM_DEFINE "_WIN32")
elseif(UNIX)
    set(PLATFORM "Unix")
    set(PLATFORM_DEFINE "_UNIX")
elseif(APPLE)
    set(PLATFORM "macOS")
    set(PLATFORM_DEFINE "_UNIX")
else()
    set(PLATFORM "Unknown")
    set(PLATFORM_DEFINE "_UNKNOWN")
endif()

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    if(BUILD_RELEASE)
        set(CMAKE_BUILD_TYPE Release)
    else()
        set(CMAKE_BUILD_TYPE Debug)
    endif()
endif()

# Build presets
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -DDEBUG")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -DDEBUG")
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Platform-specific output directories
if(WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/Debug)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/Debug)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib/Release)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/Debug)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib/Release)
endif()

# Add SQLiteCpp as a subdirectory
add_subdirectory(lib/SQLiteCpp)

# Find SQLite3 (required by SQLiteCpp)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)

# Create the random library
add_library(random_lib STATIC lib/random/random.cpp)
target_include_directories(random_lib 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib/random/include>
        $<INSTALL_INTERFACE:include>
)

# Platform-specific settings for random library
target_compile_definitions(random_lib PRIVATE ${PLATFORM_DEFINE})

# Create the words library
add_library(words_lib STATIC lib/words/words.cpp)
target_include_directories(words_lib 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib/words/include>
        $<INSTALL_INTERFACE:include>
)

# Link words_lib with SQLiteCpp
target_link_libraries(words_lib PRIVATE SQLiteCpp)

# Create the drawguess library
add_library(drawguess_lib STATIC lib/drawguess/drawguess.cpp)
target_include_directories(drawguess_lib 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib/drawguess/include>
        $<INSTALL_INTERFACE:include>
)

# Link drawguess_lib with words_lib (for Letter struct)
target_link_libraries(drawguess_lib PRIVATE words_lib)

# Main executable
add_executable(wordle-cli src/main.cpp)
target_include_directories(wordle-cli 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/SQLiteCpp/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/random/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/words/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/drawguess/include
        ${SQLITE3_INCLUDE_DIRS}
)

# Link libraries for main executable
target_link_libraries(wordle-cli
    PRIVATE
        SQLiteCpp
        random_lib
        words_lib
        drawguess_lib
        ${SQLITE3_LIBRARIES}
)

# Platform-specific link libraries for main executable
if(WIN32)
    target_link_libraries(wordle-cli PRIVATE ws2_32)
else()
    target_link_libraries(wordle-cli PRIVATE pthread dl)
endif()

# Create res/db directory structure in build directory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/res/db)

# Copy database files to build directory
file(GLOB_RECURSE DATABASE_FILES "res/db/*.db")
foreach(DB_FILE ${DATABASE_FILES})
    get_filename_component(DB_NAME ${DB_FILE} NAME)
    configure_file(${DB_FILE} ${CMAKE_BINARY_DIR}/bin/res/db/${DB_NAME} COPYONLY)
endforeach()

# Copy other resource files (if any)
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/res)
    file(GLOB_RECURSE RESOURCE_FILES "res/*")
    foreach(RES_FILE ${RESOURCE_FILES})
        get_filename_component(RES_REL_PATH ${RES_FILE} DIRECTORY)
        get_filename_component(RES_NAME ${RES_FILE} NAME)
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/res" "" RES_REL_PATH ${RES_REL_PATH})
        if(RES_REL_PATH)
            file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/res${RES_REL_PATH})
            configure_file(${RES_FILE} ${CMAKE_BINARY_DIR}/bin/res${RES_REL_PATH}/${RES_NAME} COPYONLY)
        else()
            configure_file(${RES_FILE} ${CMAKE_BINARY_DIR}/bin/res/${RES_NAME} COPYONLY)
        endif()
    endforeach()
endif()

# Test executable (conditional)
if(BUILD_TESTS)
    # Check if test.cpp exists in tests directory
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tests/test.cpp)
        set(TEST_SOURCE tests/test.cpp)
    else()
        # Fallback to lib directory if tests directory doesn't exist
        set(TEST_SOURCE lib/test.cpp)
    endif()
    
    add_executable(test-app ${TEST_SOURCE})
    target_include_directories(test-app 
        PRIVATE 
            ${CMAKE_CURRENT_SOURCE_DIR}/lib/SQLiteCpp/include
            ${CMAKE_CURRENT_SOURCE_DIR}/lib/random/include
            ${CMAKE_CURRENT_SOURCE_DIR}/lib/words/include
            ${CMAKE_CURRENT_SOURCE_DIR}/lib/drawguess/include
            ${SQLITE3_INCLUDE_DIRS}
    )

    # Link libraries for test executable
    target_link_libraries(test-app
        PRIVATE
            SQLiteCpp
            random_lib
            words_lib
            drawguess_lib
            ${SQLITE3_LIBRARIES}
    )

    # Platform-specific link libraries for test executable
    if(WIN32)
        target_link_libraries(test-app PRIVATE ws2_32)
    else()
        target_link_libraries(test-app PRIVATE pthread dl)
    endif()
    
    # Copy resources to test executable directory as well
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/res/db)
    foreach(DB_FILE ${DATABASE_FILES})
        get_filename_component(DB_NAME ${DB_FILE} NAME)
        configure_file(${DB_FILE} ${CMAKE_BINARY_DIR}/bin/res/db/${DB_NAME} COPYONLY)
    endforeach()
endif()

# Install rules
install(TARGETS wordle-cli
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

if(BUILD_TESTS)
    install(TARGETS test-app
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
    )
endif()

# Install database files
install(DIRECTORY res/
    DESTINATION share/wordle-cli/resources
    FILES_MATCHING PATTERN "*.db"
)

# Create a self-contained package with embedded databases
if(BUILD_RELEASE)
    # Copy databases to executable directory for self-contained deployment
    file(GLOB_RECURSE DATABASE_FILES "res/db/*.db")
    foreach(DB_FILE ${DATABASE_FILES})
        get_filename_component(DB_NAME ${DB_FILE} NAME)
        configure_file(${DB_FILE} ${CMAKE_BINARY_DIR}/bin/res/db/${DB_NAME} COPYONLY)
    endforeach()
    
    message(STATUS "Release build: Databases embedded in executable directory")
endif()

# Print configuration info
message(STATUS "==========================================")
message(STATUS "Wordle CLI Build Configuration")
message(STATUS "==========================================")
message(STATUS "Platform: ${PLATFORM}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "SQLite3 version: ${SQLITE3_VERSION}")
message(STATUS "Project version: ${PROJECT_VERSION}")
message(STATUS "Build tests: ${BUILD_TESTS}")
message(STATUS "Release build: ${BUILD_RELEASE}")
if(BUILD_TESTS)
    message(STATUS "Test source: ${TEST_SOURCE}")
endif()
message(STATUS "Output directory: ${CMAKE_BINARY_DIR}/bin")
message(STATUS "Resource directory: ${CMAKE_BINARY_DIR}/bin/res/db")
message(STATUS "==========================================")
