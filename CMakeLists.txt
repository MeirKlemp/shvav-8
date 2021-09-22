cmake_minimum_required(VERSION 3.16)

project(shvav-8 VERSION 0.1.0 LANGUAGES C CXX)

##################################################
# create the target and set its properties

add_executable(shvav-8)
add_subdirectory(src/)

# add src/ as include dir
target_include_directories(
  shvav-8
  PRIVATE
  src/
)

# set compile flags
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")
target_compile_options(
  shvav-8
  PRIVATE
  $<${gcc_like_cxx}:-Wall -Wextra -Wpedantic>
  $<${msvc_cxx}:-W3>
)
target_compile_features(
  shvav-8
  PRIVATE
  cxx_std_17
)

##################################################
# check dependencies exists

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw)
  message(FATAL_ERROR "Cannot find the dependency glfw. Please clone with git submodule.")
endif()

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL)
  message(FATAL_ERROR "Cannot find the dependency SDL. Please clone with git submodule.")
endif()

##################################################
# link dependencies

add_subdirectory(deps/glfw)
target_link_libraries(
  shvav-8
  PRIVATE
  glfw
)

add_subdirectory(deps/GLAD)

set(SDL_STATIC TRUE)
set(SDL_SHARED FALSE)
add_subdirectory(deps/SDL)
target_link_libraries(
  shvav-8
  PRIVATE
  SDL2main
  SDL2-static
)

##################################################
# installation instructions

install(
  TARGETS shvav-8
  DESTINATION bin/
)

##################################################
# build an installer

include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_FILE_NAME shvav-8-${CMAKE_SYSTEM_NAME})
set(CPACK_PACKAGE_VENDOR MeirKlemp)
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
include(CPack)

##################################################
