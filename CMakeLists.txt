cmake_minimum_required(VERSION 3.10)
project(linux_peripheral_library CXX)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.16.1/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
                EXPECTED_HASH SHA256=396e16d0f5eabdc6a14afddbcfff62a54a7ee75c6da23f32f7a31bc85db23484
                TLS_VERIFY ON)
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_cmake_run(CONANFILE conanfile.txt
                BASIC_SETUIP
                BUILD missing)

include_directories(${CONAN_INCLUDE_DIRS}
                    ${CMAKE_SOURCE_DIR}/third_party/etl-20.19.0/include
                    include)
link_directories(${CONAN_LIB_DIRS})

add_subdirectory(test)
