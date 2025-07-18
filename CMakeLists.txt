cmake_minimum_required(VERSION 3.16)

project(wordle-cli VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

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
if(WIN32)
    target_compile_definitions(random_lib PRIVATE _WIN32)
else()
    target_compile_definitions(random_lib PRIVATE _UNIX)
endif()

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

# Test executable
add_executable(test-app lib/test.cpp)
target_include_directories(test-app 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/SQLiteCpp/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/random/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/words/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/drawguess/include
        ${SQLITE3_LIBRARIES}
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

# Copy resources to build directory
file(COPY res DESTINATION ${CMAKE_BINARY_DIR}/bin)

# Install rules
install(TARGETS wordle-cli test-app
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY res/
    DESTINATION share/wordle-cli/resources
    FILES_MATCHING PATTERN "*.db"
)

# Print configuration info
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "SQLite3 version: ${SQLITE3_VERSION}")
message(STATUS "Project version: ${PROJECT_VERSION}") 