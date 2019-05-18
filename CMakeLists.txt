cmake_minimum_required(VERSION 3.0)

project(mechgah)

enable_testing()

# Include model sources files
file(GLOB_RECURSE
     source_files
     src/nes/* src/common/*
)

# Include Unit Test sources files
file(GLOB
     source_unit_test_files
	 src/unit-test/UTest.c
	 src/unit-test/UTcpu.c
	 src/unit-test/UTppu.c
	 src/unit-test/UTnes.c
	 src/unit-test/UTinstruction.c
	 src/unit-test/UTkeys.c
	 src/unit-test/UTioreg.c
	 src/unit-test/UTnrom.c
	 src/unit-test/UTjoypad.c
	 src/unit-test/UTcontroller.c
	 src/unit-test/UTstack.c
	 src/unit-test/UTloader.c

)

# Set CFLAGS
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")

# Search for SDL and cmocka
find_package(SDL REQUIRED)
include_directories(${SDL_INCLUDE_DIR})
find_package(CMOCKA REQUIRED)
include_directories(${CMOCKA_INCLUDE_DIR})

# Compile Mechgah executable
add_executable(mechgah main.c src/app.c src/app.h ${source_files})
target_link_libraries(mechgah ${SDL_LIBRARY} SDL_gfx)

# Compile Unit Test
add_executable(utest ${source_files} ${source_unit_test_files})
target_link_libraries(utest ${CMOCKA_LIBRARY})
add_test(utest_valgrind
		 valgrind --error-exitcode=1 --read-var-info=yes --leak-check=full
		--show-leak-kinds=all ./utest)
set_target_properties(utest PROPERTIES LINK_FLAGS "-Wl,--wrap=SDL_PollEvent -coverage")
set_target_properties(utest PROPERTIES COMPILE_FLAGS "-coverage -DDEBUG_CPU")
