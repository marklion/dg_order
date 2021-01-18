cmake_minimum_required(VERSION 3.10)
project(dg_data_base)

SET(LIBRARY_OUTPUT_PATH ${PRJ_INTERNAL_BUILD}/lib)
add_compile_options(-std=gnu++11 -g)
include_directories(${PROJECT_SOURCE_DIR})

aux_source_directory(. API_SRC)
add_library(dg_data_base SHARED ${API_SRC})
find_library(DG_SQL_ORM libsqlite_orm.so ${PRJ_INTERNAL_BUILD}/lib)
target_link_libraries(dg_data_base ${DG_SQL_ORM} uuid curl ssl)

add_executable(dg_database_test test.cpp)
target_link_libraries(dg_database_test dg_data_base)