CPMAddPackage(
  GITHUB_REPOSITORY Cyan4973/xxHash
  GIT_TAG v0.8.3
  OPTIONS "XXHASH_BUILD_ENABLE_INLINE_API OFF" "XXHASH_BUILD_XXHSUM OFF"
  SOURCE_SUBDIR cmake_unofficial
)

# Add executable
#
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/**.cpp")
set(bench_name "${PROJECT_NAME}-bench")
add_executable(${bench_name} ${sources})
target_link_libraries(${bench_name} PRIVATE ${PROJECT_NAME} nanobench hwy xxHash::xxhash)
target_compile_features(${bench_name} PRIVATE cxx_std_20)
