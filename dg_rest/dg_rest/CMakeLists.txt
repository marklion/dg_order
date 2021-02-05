cmake_minimum_required(VERSION 2.6)

project (dg_rest CXX)

set(DG_REST_HEADERS dg_rest.h)

set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(DG_REST_CODEGEN_DIR "${PROJECT_BINARY_DIR}/codegen")

PREPEND(DG_REST_HEADERS_PATHS ${PROJECT_SOURCE_DIR} ${DG_REST_HEADERS})

CODEGEN_FILES(DG_REST_CODEGEN_SOURCES ${DG_REST_CODEGEN_DIR} ${DG_REST_HEADERS})

add_custom_command(OUTPUT ${DG_REST_CODEGEN_SOURCES}
    COMMAND ${NGREST_BIN_PATH}ngrestcg -i "${PROJECT_SOURCE_DIR}" -o ${DG_REST_CODEGEN_DIR} -t service ${DG_REST_HEADERS}
    DEPENDS ${DG_REST_HEADERS_PATHS}
)

file(GLOB DG_REST_SOURCES ${PROJECT_SOURCE_DIR}/*.c*)

list(APPEND DG_REST_SOURCES ${DG_REST_CODEGEN_SOURCES})

include_directories(${PROJECT_SOURCE_DIR} $ENV{NGREST_EXT_INCLUDES})

add_library(dg_rest MODULE ${DG_REST_SOURCES})

set_target_properties(dg_rest PROPERTIES PREFIX "")
set_target_properties(dg_rest PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SERVICES_DIR}"
)

find_library(DG_DATA_BASE libdg_data_base.so ${PRJ_INTERNAL_BUILD}/lib)
find_library(SQLITE_ORM_LIB libsqlite_orm.so ${PRJ_INTERNAL_BUILD}/lib)
target_link_libraries(dg_rest ngrestutils ngrestcommon ngrestjson ngrestengine $ENV{NGREST_EXT_LIBS} uuid ${DG_DATA_BASE} ${SQLITE_ORM_LIB} curl pthread)
