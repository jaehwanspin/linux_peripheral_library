set(CMAKE_REQUIRED_LIBRARIES "-L${atomic_lib_dir}" "-latomic")
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(test_gpio gpio.cpp)
target_compile_features(test_gpio PRIVATE cxx_std_14)
target_link_libraries(test_gpio ${CONAN_LIBS} Threads::Threads atomic)

add_executable(test_pwm pwm.cpp)
target_compile_features(test_pwm PRIVATE cxx_std_14)
target_link_libraries(test_pwm ${CONAN_LIBS} Threads::Threads atomic)

add_executable(test_soft_pwm soft_pwm.cpp)
target_compile_features(test_soft_pwm PRIVATE cxx_std_14)
target_link_libraries(test_soft_pwm ${CONAN_LIBS} Threads::Threads atomic)

add_executable(test_uart uart.cpp)
target_compile_features(test_uart PRIVATE cxx_std_14)
target_link_libraries(test_uart ${CONAN_LIBS} Threads::Threads atomic)

add_executable(test_i2c i2c.cpp)
target_compile_features(test_i2c PRIVATE cxx_std_14)
target_link_libraries(test_i2c ${CONAN_LIBS} Threads::Threads atomic)

add_executable(test_led led.cpp)
target_compile_features(test_led PRIVATE cxx_std_14)
target_link_libraries(test_led ${CONAN_LIBS} Threads::Threads atomic)

add_executable(test_spi spi.cpp)
target_compile_features(test_spi PRIVATE cxx_std_14)
target_link_libraries(test_spi ${CONAN_LIBS} Threads::Threads atomic)

add_executable(test_gpio_led gpio_led.cpp)
target_compile_features(test_gpio_led PRIVATE cxx_std_14)
target_link_libraries(test_gpio_led ${CONAN_LIBS} Threads::Threads atomic)

add_executable(test_pwm_led pwm_led.cpp)
target_compile_features(test_pwm_led PRIVATE cxx_std_14)
target_link_libraries(test_pwm_led ${CONAN_LIBS} Threads::Threads atomic)