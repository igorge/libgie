cmake_minimum_required(VERSION 3.9)
project(libgie)
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(BINARY_NAME libgie)


#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_EXTENSIONS OFF)

#add_definitions(-DGIE_NO_THROW_TRACE)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Wextra -pedantic -Wno-deprecated -Wno-unused-parameter")


if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("*** Debug build. ***")
#    add_definitions(-DBOOST_MULTI_INDEX_ENABLE_SAFE_MODE)
#    add_definitions(-DBOOST_MULTI_INDEX_ENABLE_INVARIANT_CHECKING)

#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvtable-verify=std")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")

#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=float-cast-overflow")
#
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-sanitize-recover")
else()
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finline-functions -findirect-inlining")
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")
endif ()



set(SOURCE_FILES
    "src/exceptions.cpp")


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
    "tests/ut_sio2_test03.cpp"
    "tests/ut_sio2_test04.cpp"
    "tests/ut_simple_caching_allocator.cpp"
    "tests/ut_simple_lru.cpp"
    "tests/ut_caching_istream_iterator.cpp"
    "tests/ut_sbdec01.cpp")
    
find_package (Boost COMPONENTS unit_test_framework system date_time REQUIRED)
    
set(TEST test_${BINARY_NAME})
add_executable (${TEST} ${TESTS_SOURCES})
target_compile_definitions(${TEST} PRIVATE -DBOOST_TEST_DYN_LINK)
target_link_libraries (${TEST} PRIVATE ${BINARY_NAME} Boost::unit_test_framework Boost::date_time Boost::system)
enable_testing()
add_test (${TEST} ${TEST})
###


add_library(${BINARY_NAME} ${SOURCE_FILES} ${proj_headers_moc})

target_include_directories(${BINARY_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
#target_include_directories(${BINARY_NAME} PRIVATE ${Boost_INCLUDE_DIRS})


target_link_libraries(${BINARY_NAME} PRIVATE ${QT_LIBRARIES})
