cmake_minimum_required(VERSION 2.8)
project(libgie)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(proj_binary_name libgie)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -Wno-deprecated -Wno-unused-parameter")


set(SOURCE_FILES
    "src/exceptions.cpp")

### UT
add_definitions(-DBOOST_TEST_DYN_LINK)
###
   	
if(QT4_FOUND)
    message("Configuring Qt support for libgie")
    set(proj_headers include/gie/qt/qt_helper.hpp)
    QT4_WRAP_CPP(proj_headers_moc ${proj_headers})
	include(${QT_USE_FILE})
	add_definitions(${QT_DEFINITIONS})    
else()
    set(proj_headers "")
    set(proj_headers_moc "")
endif()



### UT
set(TESTS_SOURCES
    "tests/ut_next_power_of_two.cpp"
    "tests/sio2_test.cpp"
    "tests/sio2_test02.cpp"
    "tests/ut_simple_caching_allocator.cpp")
    
find_package (Boost COMPONENTS unit_test_framework system date_time REQUIRED)
    
set(TEST test_${proj_binary_name})
add_executable (${TEST} ${TESTS_SOURCES})
target_link_libraries (${TEST} ${proj_binary_name} ${Boost_LIBRARIES})
enable_testing()
add_test (${TEST} ${TEST})
###


include_directories(${Boost_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include)
add_library(${proj_binary_name} ${SOURCE_FILES} ${proj_headers_moc})
target_link_libraries(${proj_binary_name} ${QT_LIBRARIES})
