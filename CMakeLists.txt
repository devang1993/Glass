cmake_minimum_required (VERSION 3.8)

project ("Glass")

# add_subdirectory(source)

# specify parallel build processes
set( CMAKE_BUILD_PARALLEL_LEVEL 8 )

# specify toolchain file
set( CMAKE_TOOLCHAIN_FILE "../vcpkg/scripts/buildsystems/vcpkg.cmake" )

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#find_package( OpenCV REQUIRED )

include_directories(
	"../vcpkg/installed/x64-windows/include"
	"./include"
	)

# add library directories
#link_libraries(
#	"../vcpkg/installed/x64-windows/bin/"
#)

# add the executable
add_executable(
	main 
	"source/main.cpp"
	"source/rwFile.cpp"
	"source/cnn.cpp")

target_link_libraries(
	main
	 #${OpenCV_LIBS}
	 )


