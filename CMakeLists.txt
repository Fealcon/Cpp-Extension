cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdump-tree-all")

SET(PROJECT_NAME "cpp_ext")
SET(SOURCES logger
	)
set(HEADERS textuals
			logger
			numerics
	)

SET(VERSION_MAIN 0)
SET(VERSION_SUB 1.0)

include(../Libraries.cmake)
