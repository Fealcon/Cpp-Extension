cmake_minimum_required(VERSION 3.5)

project(cpp_ext VERSION 0.1.0)

add_compile_options(-fdump-tree-all)

SET(SOURCES logger.cpp
	)
set(HEADERS textuals.hpp
			logger.hpp
			numerics.hpp
			pointers.hpp
	)

include(../Libraries.cmake)
