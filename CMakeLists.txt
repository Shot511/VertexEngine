# Copyright (C) 2018 Tomasz Gałaj

cmake_minimum_required(VERSION 3.9.2 FATAL_ERROR)
project(VertexEngine VERSION 0.1)

option(BUILD_EXAMPLE_GAME 
	   "Build example game project to see Vertex Engine in action."
	   ON)

set(THIRDPARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")

# add thirdparties
include(thirdparty/thirdparty.cmake)

# subdirectories
add_subdirectory(src)

if(BUILD_EXAMPLE_GAME)
	message(STATUS "Creating Example Game Project")
	add_subdirectory(example_game)
endif()