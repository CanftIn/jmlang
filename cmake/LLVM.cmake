##
# Find LLVM and check the version.
##

include(CMakeDependentOption)
include(BundleStatic)

# Fallback configurations for weirdly built LLVMs
set(CMAKE_MAP_IMPORTED_CONFIG_MINSIZEREL MinSizeRel Release RelWithDebInfo "")
set(CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO RelWithDebInfo Release MinSizeRel "")
set(CMAKE_MAP_IMPORTED_CONFIG_RELEASE Release MinSizeRel RelWithDebInfo "")

find_package(LLVM ${JMLANG_REQUIRE_LLVM_VERSION} REQUIRED)
find_package(Clang REQUIRED CONFIG HINTS "${LLVM_DIR}/../clang" "${LLVM_DIR}/../lib/cmake/clang")

set(LLVM_PACKAGE_VERSION "${LLVM_PACKAGE_VERSION}"
    CACHE INTERNAL "LLVM version")

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "Using ClangConfig.cmake in: ${Clang_DIR}")

separate_arguments(LLVM_DEFINITIONS NATIVE_COMMAND "${LLVM_DEFINITIONS}")
set(JMLANG_LLVM_DEFS ${LLVM_DEFINITIONS} $<BUILD_INTERFACE:LLVM_VERSION=${LLVM_VERSION_MAJOR}${LLVM_VERSION_MINOR}>)

# Note, removing -D_GLIBCXX_ASSERTIONS is a workaround for https://reviews.llvm.org/D142279
list(REMOVE_ITEM JMLANG_LLVM_DEFS "-D_GLIBCXX_ASSERTIONS")

##
# Options for which version of LLVM to use
##

option(JMLANG_SHARED_LLVM "Link against shared LLVM (ignores components)." OFF)
cmake_dependent_option(JMLANG_BUNDLE_LLVM "When built as a static library, include LLVM's objects." OFF
                       "NOT BUILD_SHARED_LIBS" OFF)

##
# Promote LLVM/Clang executable targets
##

set_target_properties(llvm-as clang PROPERTIES IMPORTED_GLOBAL TRUE)

# clang-tools-extra is optional, but provides the clang-format target
if (TARGET clang-format)
  set_target_properties(clang-format PROPERTIES IMPORTED_GLOBAL TRUE)
endif ()

##
# Create options for including or excluding LLVM backends.
##

set(active_components orcjit bitwriter linker passes)
set(known_components AArch64 AMDGPU ARM Hexagon NVPTX PowerPC RISCV WebAssembly X86)

foreach (comp IN LISTS known_components)
  string(TOUPPER "TARGET_${comp}" OPTION)
  string(TOUPPER "WITH_${comp}" DEFINE)

  if (comp STREQUAL "RISCV" AND LLVM_PACKAGE_VERSION VERSION_LESS 17.0)
    # We default the RISCV target to OFF for LLVM versions prior to 17.0;
    # it's not clear how robust and well-tested JMLANG's RISCV codegen
    # is with LLVM16, and a great deal of effort is being put into
    # improving it in LLVM17... so default to off so that people won't
    # hurt themselves too badly.
    cmake_dependent_option(${OPTION} "Include ${comp} target" OFF
                            "${comp} IN_LIST LLVM_TARGETS_TO_BUILD" OFF)
  else ()
    cmake_dependent_option(${OPTION} "Include ${comp} target" ON
                            "${comp} IN_LIST LLVM_TARGETS_TO_BUILD" OFF)
  endif ()
  if (${OPTION} OR JMLANG_SHARED_LLVM)
    message(STATUS "Enabling ${comp} backend")
    list(APPEND JMLANG_LLVM_DEFS $<BUILD_INTERFACE:${DEFINE}>)
    list(APPEND active_components ${comp})
  else ()
    message(STATUS "Disabling ${comp} backend")
  endif ()
endforeach ()

set(wasm_libs "")
if (TARGET_WEBASSEMBLY)
  find_package(LLD CONFIG REQUIRED HINTS "${LLVM_DIR}/../lld" "${LLVM_DIR}/../lib/cmake/lld")
  message(STATUS "Using LLDConfig.cmake in: ${LLD_DIR}")

  if (LLVM_LINK_LLVM_DYLIB AND NOT JMLANG_SHARED_LLVM)
    message(FATAL_ERROR "LLD was linked to shared LLVM (see: LLVM_LINK_LLVM_DYLIB), "
            "but static LLVM was requested. Re-configure with JMLANG_SHARED_LLVM=YES "
            "to enable WebAssembly, or disable WebAssembly with TARGET_WEBASSEMBLY=OFF.")
  endif ()

  set(wasm_libs lldWasm)
endif ()

##
# Create JMLANG::LLVM library alias pointing to the correct LLVM
# among shared, static, and bundled.
##

add_library(JMLANG_LLVM INTERFACE)
add_library(JMLANG::LLVM ALIAS JMLANG_LLVM)

set_target_properties(JMLANG_LLVM PROPERTIES EXPORT_NAME LLVM)
target_include_directories(JMLANG_LLVM INTERFACE "$<BUILD_INTERFACE:${LLVM_INCLUDE_DIRS}>")
target_compile_definitions(JMLANG_LLVM INTERFACE ${JMLANG_LLVM_DEFS})

# Link LLVM libraries to JMLANG_LLVM, depending on shared, static, or bundled selection.
if (JMLANG_SHARED_LLVM)
  target_link_libraries(JMLANG_LLVM INTERFACE LLVM ${wasm_libs} ${CMAKE_DL_LIBS})
else ()
  llvm_map_components_to_libnames(llvm_targets ${active_components})
  list(APPEND llvm_targets ${wasm_libs})
  if (JMLANG_BUNDLE_LLVM)
    bundle_static(JMLANG_LLVM LIBRARIES ${llvm_targets})
  else ()
    target_link_libraries(JMLANG_LLVM INTERFACE ${llvm_targets})
  endif ()
endif ()

##
# Language options interface library
##

add_library(JMLANG_LanguageOptions INTERFACE)
add_library(JMLANG::LanguageOptions ALIAS JMLANG_LanguageOptions)

set_target_properties(JMLANG_LanguageOptions PROPERTIES EXPORT_NAME LanguageOptions)

option(JMLANG_ENABLE_RTTI "Enable RTTI" ${LLVM_ENABLE_RTTI})
if (JMLANG_ENABLE_RTTI AND NOT LLVM_ENABLE_RTTI)
  message(FATAL_ERROR "Can't enable RTTI. LLVM was compiled without it")
endif ()

if (JMLANG_ENABLE_RTTI)
  message(STATUS "Compiling JMLANG WITH RTTI.")
  target_compile_definitions(JMLANG_LanguageOptions INTERFACE JMLANG_ENABLE_RTTI)
else ()
  message(STATUS "Compiling JMLANG WITHOUT RTTI.")
  target_compile_options(JMLANG_LanguageOptions INTERFACE
                         $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/GR->
                         $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-fno-rtti>)
endif ()

option(JMLANG_ENABLE_EXCEPTIONS "Enable exceptions" ON)
if (JMLANG_ENABLE_EXCEPTIONS)
  message(STATUS "Compiling JMLANG WITH exceptions.")
  target_compile_definitions(JMLANG_LanguageOptions INTERFACE JMLANG_WITH_EXCEPTIONS)
else ()
  message(STATUS "Compiling JMLANG WITHOUT exceptions.")
  target_compile_options(JMLANG_LanguageOptions INTERFACE
                         $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/EHs-c->
                         $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-fno-exceptions>)
  target_compile_definitions(JMLANG_LanguageOptions INTERFACE
                             $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:_HAS_EXCEPTIONS=0>)
endif ()

if (LLVM_LIBCXX GREATER -1)
  message(STATUS "LLVM linked to libc++. Adding to interface requirements.")
  target_compile_options(JMLANG_LanguageOptions INTERFACE
                         $<$<STREQUAL:$<TARGET_PROPERTY:LINKER_LANGUAGE>,CXX>:-stdlib=libc++>)
  target_link_options(JMLANG_LanguageOptions INTERFACE
                      $<$<STREQUAL:$<TARGET_PROPERTY:LINKER_LANGUAGE>,CXX>:-stdlib=libc++>)
endif ()
