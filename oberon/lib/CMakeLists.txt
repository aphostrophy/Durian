cmake_minimum_required(VERSION 3.0)
project(LibOberon C)
add_library(oberon_def STATIC oberon_def_interface.c)
install(TARGETS oberon_def DESTINATION .)