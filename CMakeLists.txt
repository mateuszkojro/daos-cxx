cmake_minimum_required(VERSION 3.10)

project(daos-cxx
        DESCRIPTION "DAOS bindings for c++"
        LANGUAGES C CXX
)

# TODO: Add option to specify daos install location
option(WITH_DAOS "Should the library be linked against libdaos" "ON")
option(WITH_OPENTELEMETRY "Should the library be linked against opentelemetry-cpp and zipkin" "OFF")
option(EXTERNAL_DAOS_DIR "Installed DAOS directory" "OFF")

if(${EXTERNAL_DAOS_DIR} STREQUAL "OFF")
    set(DAOS_DIR ${PROJECT_SOURCE_DIR}/lib/daos/install/)    
else()
    set(DAOS_DIR ${EXTERNAL_DAOS_DIR})    
endif()

message("DAOS directory set to: " ${DAOS_DIR})
add_library(daos-cxx
    # Implemented interfaces
    include/interfaces.h

    # Library
    include/Pool.h src/Pool.cxx
    include/Container.h src/Container.cxx
    include/Errors.h
    include/EventQueue.h src/EventQueue.cxx
    include/KeyValue.h src/KeyValue.cxx
    include/DAOSObject.h src/DAOSObject.cxx
    include/Array.h src/Array.cxx
    include/UUID.h src/UUID.cxx

# TODO: Add option to use mocking
    # Mocking
    # include/DAOSZipkinLog.h
    # include/mocking.h
    # include/MockPool.h
    # include/MockContainer.h
    # include/MockKeyValue.h
)

set(DAOS_CXX_INCLUDES include/)

if(${WITH_DAOS})
  if(${EXTERNAL_DAOS_DIR} STREQUAL "OFF")
    message(CHECK_START "Compiling libdaos: ")
    include(ProcessorCount)
    find_program(SCONS_EXECUTABLE NAMES scons-3 scons REQUIRED)
    ProcessorCount(CORE_COUNT)
    message("Found scons-3 executable: ${SCONS_EXECUTABLE}")
    execute_process(COMMAND ${SCONS_EXECUTABLE} ". --build-deps=yes --config=force -j ${CORE_COUNT}"
                  WORKING_DIRECTORY ./lib/daos
                  COMMAND_ERROR_IS_FATAL ANY
                )
    message(CHECK_PASS "done")
  endif()
  target_include_directories(daos-cxx PUBLIC ${DAOS_DIR}/include/ ${DAOS_CXX_INCLUDES})
  target_link_libraries(daos-cxx PUBLIC 
      ${DAOS_DIR}/lib64/libdaos.so 
      ${DAOS_DIR}/lib64/libgurt.so 
      ${DAOS_DIR}/lib64/libdaos_common.so 
      uuid
  )
endif()
if(${WITH_OPENTELEMETRY})
    set(opentelemetry ./lib/opentelemetry-cpp)
    add_subdirectory(${opentelemetry})
    set(OPENTELEMETRY_LIBS PUBLIC ${CMAKE_THREAD_LIBS_INIT} opentelemetry_trace opentelemetry_exporter_zipkin_trace)
    target_include_directories(daos-cxx PUBLIC ${opentelemetry}/exporters/zipkin/include ${opentelemetry} ${DAOS_CXX_INCLUDES})
    target_link_libraries(daos-cxx PUBLIC 
        ${OPENTELEMETRY_LIBS}
        uuid
    )
endif()

if(NOT ${WITH_DAOS})
	add_compile_definitions(DAOS_CXX_MOCK)
endif()


# Add definitions for conditional compilation
add_compile_definitions(MK_WITH_DAOS MK_WITH_OPENTELEMETRY)
