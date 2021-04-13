file(GLOB UNIT_TEST_SL_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_executable(unitTestSocketLib ${UNIT_TEST_SL_SRC_FILES})

target_link_libraries (unitTestSocketLib LINK_PUBLIC socketLib.a cppunit)
