cmake_minimum_required(VERSION 3.22)
project(my_xl)

set(CMAKE_CXX_STANDARD 14)
add_definitions("-Wall -Wno-sign-compare -O3 -std=c++11
-march=native -Wno-strict-aliasing -Wno-comment")
add_executable(my_xl main.cpp base/timer.h base/common.h base/logging.h base/timer.cc base/logging.cc solver/solver.h solver/solver.cpp data/hyper_parameters.h data/data_structure.h base/file_util.h base/scoped_ptr.h base/stringprintf.h base/stringprintf.cpp solver/checker.h solver/checker.cpp base/format_print.h base/levenshtein_distance.h base/levenshtein_distance.cpp data/model_parameters.h data/model_parameters.cpp base/math.h base/thread_pool.h reader/reader.h reader/reader.cpp base/class_register.h reader/parser.h reader/parser.cpp base/split_string.h base/split_string.cpp reader/file_splitor.h reader/file_splitor.cpp reader/mman.h reader/mman.cpp score/score_function.h score/score_function.cpp score/linear_score.h score/linear_score.cpp score/fm_score.h score/fm_score.cpp score/ffm_score.h score/ffm_score.cpp loss/loss.h loss/loss.cpp loss/squared_loss.h loss/squared_loss.cpp loss/cross_entropy_loss.h loss/cross_entropy_loss.cpp loss/metric.h loss/metric.cpp solver/trainer.h solver/trainer.cpp solver/inference.h solver/inference.cpp base/system.h base/utsname.h base/utsname.cpp)
