cmake_minimum_required (VERSION 3.8)

project ("Glass")

# add_subdirectory(source)

# specify parallel build processes
set( CMAKE_BUILD_PARALLEL_LEVEL 8 )

# specify toolchain file
set( CMAKE_TOOLCHAIN_FILE "../vcpkg/scripts/buildsystems/vcpkg.cmake" )

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(
	"../vcpkg/installed/x64-windows/include"
	"include/glass.h"
	"include/readFile.h"
	"include/rwFile.h"
	)

# add the executable
add_executable(
	main 
	"source/main.cpp"
	#"source/rwFile.cpp"
	)

# target_link_libraries(main)


