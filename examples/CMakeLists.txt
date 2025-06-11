# Add executable
#

CPMAddPackage("gh:fmtlib/fmt#11.0.2")

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
set(example_name "${PROJECT_NAME}-ex")
add_executable(${example_name} ${sources})
target_link_libraries(${example_name} PRIVATE ${PROJECT_NAME} hwy fmt::fmt)
target_compile_features(${example_name} PRIVATE cxx_std_20)
