set(CMAKE_BUILD_TYPE Debug)

option(ENABLE_LOG "Enable logging" OFF)

# Source files in src 
set(DD_FILES digitdodo.cpp)

pkg_check_modules(DIGITDODO_PLATFORM REQUIRED libdigitdodoplatform)
pkg_check_modules(TICKTOUCAN_PLATFORM REQUIRED libticktoucanplatform)
pkg_check_modules(TICKTOUCAN REQUIRED libticktoucan)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/platform_contracts)
include_directories(${TICKTOUCAN_INCLUDE_DIRS})
include_directories(${TICKTOUCAN_PLATFORM_INCLUDE_DIRS})
include_directories(${DIGITDODO_PLATFORM_INCLUDE_DIRS})



# Create the main library from src 
add_library(digitdodo SHARED ${DD_FILES})

if(ENABLE_LOG)
    message(STATUS "Logging enabled")
    add_compile_definitions(ENABLE_LOG)
else()
    message(STATUS "Logging disabled")
endif()


target_include_directories(digitdodo PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(digitdodo ${DIGITDODO_PLATFORM_LINK_LIBRARIES})
target_link_libraries(digitdodo ${TICKTOUCAN_PLATFORM_LINK_LIBRARIES})
target_link_libraries(digitdodo ${TICKTOUCAN_LINK_LIBRARIES})

install(TARGETS digitdodo
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

install(FILES ${CMAKE_SOURCE_DIR}/include/digitdodo.h ${CMAKE_SOURCE_DIR}/include/platform_contracts/digitdodo_platform.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include)