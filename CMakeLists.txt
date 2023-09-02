cmake_minimum_required(VERSION 3.22.1)
project(prompts_cpp)


add_library(prompts_cpp SHARED src/prompts.h)

add_executable(prompts_cpp_test src/tests.cc)
set_target_properties(prompts_cpp PROPERTIES LINKER_LANGUAGE CXX)
