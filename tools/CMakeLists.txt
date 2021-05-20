add_executable(mcs mcs.cpp)
target_link_libraries (mcs LINK_PUBLIC socketLib)

add_executable(mcr mcr.cpp)
target_link_libraries (mcr LINK_PUBLIC socketLib)
