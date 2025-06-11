#####################################
# compile & link options
#####################################
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
set(BUILD_SHARED_LIBS OFF)

# # Default to -O2 on release builds.
# if(CMAKE_CXX_FLAGS_RELEASE MATCHES "-O3")
#   message(STATUS "Replacing -O3 in CMAKE_C_FLAGS_RELEASE with -O2")
#   string(REPLACE "-O3" "-O2" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
#   string(REPLACE "-O3" "-O2" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
# endif()

# being a cross-platform target, we enforce standards conformance on MSVC all compile:
# https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_COMPILER_ID.html
if (LINUX)
  if (NOT CMAKE_CROSSCOMPILING)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wno-narrowing")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native -Wno-narrowing")
  endif()
elseif(MSVC)
  set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} /permissive-")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /permissive-")
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

# lto
include(CheckIPOSupported)
macro(link_time_optimization)
  # Argument parsing
  set(options REQUIRED)
  set(single_value_keywords)
  set(multi_value_keywords)
  cmake_parse_arguments(link_time_optimization "${options}" "${single_value_keywords}" "${multi_value_keywords}"
                        ${ARGN})

  check_ipo_supported(RESULT result OUTPUT output)
  if(result)
    # It's available, set it for all following items
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF)
  else()
    if(link_time_optimization_REQUIRED)
      message(FATAL_ERROR "Link Time Optimization not supported, but listed as REQUIRED: ${output}")
    else()
      message(WARNING "Link Time Optimization not supported: ${output}")
    endif()
  endif()
endmacro()

cmake_policy(SET CMP0069 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)
link_time_optimization()

#####################################
# util
#####################################
macro(_clear_variable)
  cmake_parse_arguments(CLEAR_VAR "" "DESTINATION;BACKUP;REPLACE" "" ${ARGN})
  set(${CLEAR_VAR_BACKUP} ${${CLEAR_VAR_DESTINATION}})
  set(${CLEAR_VAR_DESTINATION} ${CLEAR_VAR_REPLACE})
endmacro()

macro(_restore_variable)
  cmake_parse_arguments(CLEAR_VAR "" "DESTINATION;BACKUP" "" ${ARGN})
  set(${CLEAR_VAR_DESTINATION} ${${CLEAR_VAR_BACKUP}})
  unset(${CLEAR_VAR_BACKUP})
endmacro()
