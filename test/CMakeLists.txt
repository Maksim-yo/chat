
add_executable(tests
               utils/UtilsTest.hpp
               utils/UtilsTest.cpp)

target_link_libraries(tests PRIVATE oatpp::oatpp PRIVATE oatpp::oatpp-test)

target_include_directories(tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_test(tests tests)