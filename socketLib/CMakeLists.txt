# Create a library called "Hello" which includes the source file "hello.cxx".
# The extension is already found. Any number of sources could be listed here.
file(GLOB SL_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_library (socketLib ${SL_SRC_FILES})

# Make sure the compiler can find include files for socketLib.a library
# when other libraries or executables link to socketLib.a
target_include_directories (socketLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(UnitTest)
