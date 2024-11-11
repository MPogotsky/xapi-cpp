set(XAPI_PUBLIC_H
    Enums.hpp
    Exceptions.hpp
    IConnection.hpp
    Connection.hpp
    XStationClient.hpp
    XStationClientStream.hpp
    Xapi.hpp
)

set(XAPI_SOURCES
    ${XAPI_PUBLIC_H}
    Connection.cpp
    XStationClient.cpp
    XStationClientStream.cpp
)

add_library(Xapi SHARED ${XAPI_SOURCES})

# TARGET SETUP OPTIONS ========================================
set(COMMON_FLAGS
    -Wall
    -Werror
    -Wpedantic
    -Wextra
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    list(APPEND COMMON_FLAGS -O3)
else()
    list(APPEND COMMON_FLAGS -O0 -g)
endif()
target_compile_options(Xapi PRIVATE ${COMMON_FLAGS})

# TARGET LINK OPTIONS ========================================
target_link_libraries(Xapi PRIVATE
    Boost::system
    Boost::url
    Boost::json
    OpenSSL::SSL
    OpenSSL::Crypto
)

# LIBRARY SETUP OPTIONS ========================================
include(GNUInstallDirs)

target_include_directories(Xapi
    PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

install(TARGETS Xapi
    EXPORT XapiTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES ${XAPI_PUBLIC_H} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xapi)

install(EXPORT XapiTargets
    FILE XapiTargets.cmake
    NAMESPACE Xapi::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Xapi
)

# FIND PACKAGE ORCHESTRATION ====================================
include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/XapiConfig.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Xapi
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/XapiConfig.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Xapi
)

