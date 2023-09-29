include_directories(../../include)
aux_source_directory(. SRCS)
add_library(reactor STATIC ${SRCS})
set_target_properties(reactor PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../lib)