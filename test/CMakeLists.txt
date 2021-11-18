add_executable(test_gpio gpio.cpp)
target_compile_features(test_gpio PRIVATE cxx_std_17)
target_link_libraries(test_gpio ${CONAN_LIBS})

add_executable(test_pwm pwm.cpp)
target_compile_features(test_pwm PRIVATE cxx_std_17)
target_link_libraries(test_pwm ${CONAN_LIBS})

add_executable(test_soft_pwm soft_pwm.cpp)
target_compile_features(test_soft_pwm PRIVATE cxx_std_17)
target_link_libraries(test_soft_pwm ${CONAN_LIBS})

add_executable(test_uart uart.cpp)
target_compile_features(test_uart PRIVATE cxx_std_17)
target_link_libraries(test_uart ${CONAN_LIBS})
