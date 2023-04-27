cmake_minimum_required(VERSION 3.0)
project(LibDurian C)
add_library(durian_def STATIC durian_def_interface.c)
install(TARGETS durian_def DESTINATION .)