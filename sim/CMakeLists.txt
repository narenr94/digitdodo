set(CMAKE_BUILD_TYPE Debug)

find_package(PkgConfig REQUIRED)

pkg_check_modules(DIGITDODO REQUIRED libdigitdodo)
pkg_check_modules(DIGITDODO_PLATFORM REQUIRED libdigitdodoplatform)
pkg_check_modules(TICKTOUCAN_PLATFORM REQUIRED libticktoucanplatform)
pkg_check_modules(TICKTOUCAN REQUIRED libticktoucan)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Source files in src 
set(DD_FILES digitdodo_sim.cpp)

include_directories(${CMAKE_SOURCE_DIR}/../include/platform_contracts)
include_directories(${CMAKE_SOURCE_DIR}/../include/digitDisplay)
include_directories(${CMAKE_SOURCE_DIR}/../include/)
include_directories(${TICKTOUCAN_INCLUDE_DIRS})
include_directories(${TICKTOUCAN_PLATFORM_INCLUDE_DIRS})
include_directories(${DIGITDODO_PLATFORM_INCLUDE_DIRS})
include_directories(${DIGITDODO_INCLUDE_DIRS})

# Create the main executable from src 
add_executable(digitdodo ${DD_FILES})

target_link_libraries(digitdodo ${DIGITDODO_LINK_LIBRARIES})
target_link_libraries(digitdodo ${DIGITDODO_PLATFORM_LINK_LIBRARIES})
target_link_libraries(digitdodo PRIVATE sfml-graphics sfml-window sfml-system)
target_link_libraries(digitdodo ${TICKTOUCAN_PLATFORM_LINK_LIBRARIES})
target_link_libraries(digitdodo ${TICKTOUCAN_LINK_LIBRARIES})

install(TARGETS digitdodo
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)
