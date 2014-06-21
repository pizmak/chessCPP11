# Make sure the compiler can find include files from our Hello library. 
include_directories (${CMAKE_SOURCE_DIR}/src) 

# Make sure the linker can find the Hello library once it is built. 
#    link_directories (${HELLO_BINARY_DIR}/Hello) 

# Add executable called "helloDemo" that is built from the source files 
# "demo.cxx" and "demo_b.cxx". The extensions are automatically found. 
FILE(GLOB plikiCpp *.cpp utils/*.cpp test/*.cpp zobristHashing/*.cpp)
add_executable (szachy ${plikiCpp}) 
add_library (szachylib ${plikiCpp}) 

# Link the executable to the Hello library. 
#$    target_link_libraries (helloDemo Hello) 

