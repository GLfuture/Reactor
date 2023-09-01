aux_source_directory(. SRCS)
add_library(reactor SHARED ${SRCS})
link_directories(../lib)
target_link_libraries(reactor server)