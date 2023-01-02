cmake_minimum_required(VERSION 3.22)
project(my_xl)

set(CMAKE_CXX_STANDARD 14)

add_executable(my_xl main.cpp base/timer.h base/common.h base/logging.h base/timer.cc base/logging.cc solver/solver.h solver/solver.cpp data/hyper_parameters.h data/data_structure.h base/file_util.h base/scoped_ptr.h base/stringprintf.h base/stringprintf.cpp solver/checker.h solver/checker.cpp base/format_print.h base/levenshtein_distance.h base/levenshtein_distance.cpp data/model_parameters.h data/model_parameters.cpp base/math.h)
