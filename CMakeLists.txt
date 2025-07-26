cmake_minimum_required(VERSION 3.16)

project(wordle-cli VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build test executable" OFF)
option(BUILD_RELEASE "Build in release mode" ON)

# Detect platform
if(WIN32)
    set(PLATFORM "Windows")
    set(PLATFORM_DEFINE "_WIN32")
elseif(APPLE)
    set(PLATFORM "macOS")
    set(PLATFORM_DEFINE "_UNIX")
elseif(UNIX)
    set(PLATFORM "Unix")
    set(PLATFORM_DEFINE "_UNIX")
else()
    set(PLATFORM "Unknown")
    set(PLATFORM_DEFINE "_UNKNOWN")
endif()

# Build type default
if(NOT CMAKE_BUILD_TYPE)
    if(BUILD_RELEASE)
        set(CMAKE_BUILD_TYPE Release)
    else()
        set(CMAKE_BUILD_TYPE Debug)
    endif()
endif()

# Compiler flags
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -DDEBUG")
endif()

# Output folders
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Use SQLiteCpp and SQLite3 from vcpkg
find_package(SQLiteCpp REQUIRED)
find_package(SQLite3 REQUIRED)

# === Libraries ===

# random_lib
add_library(random_lib STATIC lib/random/random.cpp)
target_include_directories(random_lib PUBLIC lib/random/include)
target_compile_definitions(random_lib PRIVATE ${PLATFORM_DEFINE})

# words_lib
add_library(words_lib STATIC lib/words/words.cpp)
target_include_directories(words_lib PUBLIC lib/words/include)
target_link_libraries(words_lib PRIVATE SQLiteCpp SQLite::SQLite3)

# drawguess_lib
add_library(drawguess_lib STATIC lib/drawguess/drawguess.cpp)
target_include_directories(drawguess_lib PUBLIC lib/drawguess/include)
target_link_libraries(drawguess_lib PRIVATE words_lib)

# === Executable ===

add_executable(wordle-cli src/main.cpp)
target_link_libraries(wordle-cli PRIVATE random_lib words_lib drawguess_lib SQLiteCpp SQLite::SQLite3)

# Platform-specific linking
if(WIN32)
    target_link_libraries(wordle-cli PRIVATE ws2_32)
else()
    target_link_libraries(wordle-cli PRIVATE pthread dl)
endif()

# === Resources ===

# Create res/db directory structure
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res/db)

# Copy database files
file(GLOB_RECURSE DATABASE_FILES "res/db/*.db")
foreach(DB_FILE ${DATABASE_FILES})
    get_filename_component(DB_NAME ${DB_FILE} NAME)
    configure_file(${DB_FILE} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res/db/${DB_NAME} COPYONLY)
endforeach()

# Copy other resource files
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/res)
    file(GLOB_RECURSE RESOURCE_FILES "res/*")
    foreach(RES_FILE ${RESOURCE_FILES})
        get_filename_component(RES_REL_PATH ${RES_FILE} DIRECTORY)
        get_filename_component(RES_NAME ${RES_FILE} NAME)
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/res" "" RES_REL_PATH ${RES_REL_PATH})
        if(RES_REL_PATH)
            file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res${RES_REL_PATH})
            configure_file(${RES_FILE} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res${RES_REL_PATH}/${RES_NAME} COPYONLY)
        else()
            configure_file(${RES_FILE} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res/${RES_NAME} COPYONLY)
        endif()
    endforeach()
endif()

# === Tests ===

if(BUILD_TESTS)
    set(TEST_SOURCE
        $<IF:$<BOOL:${EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tests/test.cpp}>,tests/test.cpp,lib/test.cpp>
    )

    add_executable(test-app ${TEST_SOURCE})
    target_link_libraries(test-app PRIVATE random_lib words_lib drawguess_lib SQLiteCpp SQLite::SQLite3)

    if(WIN32)
        target_link_libraries(test-app PRIVATE ws2_32)
    else()
        target_link_libraries(test-app PRIVATE pthread dl)
    endif()
endif()

# === Install ===

install(TARGETS wordle-cli RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

if(BUILD_TESTS)
    install(TARGETS test-app RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
endif()

install(DIRECTORY res/ DESTINATION share/wordle-cli/resources FILES_MATCHING PATTERN "*.db")

# === Messages ===
message(STATUS "==========================================")
message(STATUS "Wordle CLI Build Configuration")
message(STATUS "Platform: ${PLATFORM}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Project version: ${PROJECT_VERSION}")
message(STATUS "Build tests: ${BUILD_TESTS}")
message(STATUS "Release build: ${BUILD_RELEASE}")
message(STATUS "Output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "Resource directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res/db")
message(STATUS "==========================================")
