cmake_minimum_required (VERSION 3.31)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)
project (CSVReader)

include_directories(include)
add_executable(main
    main.cpp
    src/ui.cpp
    src/option_pricer.cpp
    src/european_pricer.cpp
    src/binary_pricer.cpp
    src/barrier_pricer.cpp
    src/asian_pricer.cpp
    src/lookback_pricer.cpp
    src/model.cpp
)



target_include_directories(main PRIVATE include)
add_executable(tests_bin tests/mc_pricer.cpp)
target_include_directories(tests_bin PRIVATE include)
