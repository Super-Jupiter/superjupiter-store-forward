cmake_minimum_required(VERSION 3.16)
project(SuperJupiterStoreForward VERSION 0.1.0 LANGUAGES CXX)

set(_superjupiter_store_forward_top_level OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(_superjupiter_store_forward_top_level ON)
endif()
option(SUPERJUPITER_STORE_FORWARD_BUILD_TESTS "Build StoreForward tests" ${_superjupiter_store_forward_top_level})
option(SUPERJUPITER_STORE_FORWARD_BUILD_EXAMPLES "Build StoreForward host example" ${_superjupiter_store_forward_top_level})
option(SUPERJUPITER_STORE_FORWARD_INSTALL "Generate StoreForward install rules" ${_superjupiter_store_forward_top_level})

add_library(SuperJupiterStoreForward INTERFACE)
add_library(SuperJupiter::StoreForward ALIAS SuperJupiterStoreForward)
set_target_properties(SuperJupiterStoreForward PROPERTIES EXPORT_NAME StoreForward)
target_include_directories(SuperJupiterStoreForward INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:include>)
target_compile_features(SuperJupiterStoreForward INTERFACE cxx_std_11)

if(SUPERJUPITER_STORE_FORWARD_BUILD_TESTS)
  enable_testing()
  add_executable(superjupiter-store-forward-tests tests/test.cpp)
  target_link_libraries(superjupiter-store-forward-tests PRIVATE SuperJupiter::StoreForward)
  add_test(NAME superjupiter-store-forward-tests COMMAND superjupiter-store-forward-tests)
  add_executable(superjupiter-store-forward-namespace-test tests/namespace_isolation.cpp)
  target_link_libraries(superjupiter-store-forward-namespace-test PRIVATE SuperJupiter::StoreForward)
  add_test(NAME superjupiter-store-forward-namespace-test COMMAND superjupiter-store-forward-namespace-test)
endif()
if(SUPERJUPITER_STORE_FORWARD_BUILD_EXAMPLES)
  add_executable(superjupiter-store-forward-replay tools/replay.cpp)
  target_link_libraries(superjupiter-store-forward-replay PRIVATE SuperJupiter::StoreForward)
endif()
if(SUPERJUPITER_STORE_FORWARD_INSTALL)
  include(CMakePackageConfigHelpers)
  set(_superjupiter_store_forward_package_dir lib/cmake/SuperJupiterStoreForward)
  install(TARGETS SuperJupiterStoreForward EXPORT SuperJupiterStoreForwardTargets)
  install(FILES src/SuperJupiterStoreForward.h DESTINATION include)
  install(EXPORT SuperJupiterStoreForwardTargets NAMESPACE SuperJupiter::
    DESTINATION ${_superjupiter_store_forward_package_dir})
  configure_package_config_file(cmake/SuperJupiterStoreForwardConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/SuperJupiterStoreForwardConfig.cmake
    INSTALL_DESTINATION ${_superjupiter_store_forward_package_dir})
  write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/SuperJupiterStoreForwardConfigVersion.cmake
    VERSION ${PROJECT_VERSION} COMPATIBILITY SameMinorVersion ARCH_INDEPENDENT)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/SuperJupiterStoreForwardConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/SuperJupiterStoreForwardConfigVersion.cmake
    DESTINATION ${_superjupiter_store_forward_package_dir})
  install(FILES LICENSE DESTINATION share/licenses/SuperJupiterStoreForward)
endif()
