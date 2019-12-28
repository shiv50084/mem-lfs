set(lfs_prefix ${CMAKE_CURRENT_LIST_DIR}/littlefs)
set(lfs_src ${lfs_prefix}/lfs.c ${lfs_prefix}/lfs_util.c)

add_library(lfs OBJECT ${lfs_src})
target_include_directories(lfs PUBLIC ${lfs_prefix})