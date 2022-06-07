cmake_minimum_required(VERSION 3.10)

project(daos-cxx
        DESCRIPTION "DAOS bindings for c++"
        LANGUAGES C CXX
)

option(WITH_DAOS "Should the library be linked against libdaos" "ON")
option(DAOS_DIR "Location of installed libdaos - defaults to submodule")
option(WITH_OPENTELEMETRY "Should the library be linked against opentelemetry-cpp and zipkin" "OFF")

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

    # Mocking
    include/DAOSZipkinLog.h
    include/mocking.h
    include/MockPool.h
    include/MockContainer.h
    include/MockKeyValue.h
)

set(DAOS_CXX_INCLUDES include/)

if(${WITH_DAOS})
  target_include_directories(daos-cxx PUBLIC ${DAOS_DIR}/include/ ${DAOS_CXX_INCLUDES})
  target_link_libraries(daos-cxx PUBLIC 
      ${DAOS_DIR}/lib64/libdaos.so 
      ${DAOS_DIR}/lib64/libgurt.so 
      ${DAOS_DIR}/lib64/libdaos_common.so 
      uuid
  )
elseif(${WITH_OPENTELEMETRY})
    set(opentelemetry ./lib/opentelemetry-cpp)
    add_subdirectory(${opentelemetry})
    set(OPENTELEMETRY_LIBS PUBLIC ${CMAKE_THREAD_LIBS_INIT} opentelemetry_trace opentelemetry_exporter_zipkin_trace)
    target_include_directories(daos-cxx PUBLIC ${opentelemetry}/exporters/zipkin/include ${opentelemetry} ${DAOS_CXX_INCLUDES})
    target_link_libraries(daos-cxx PUBLIC 
        ${OPENTELEMETRY_LIBS}
        uuid
    )
endif()


# Add definitions for conditional compilation
add_compile_definitions(MK_WITH_DAOS MK_WITH_OPENTELEMETRY)
