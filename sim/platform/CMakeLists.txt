set(CMAKE_BUILD_TYPE Debug)

# Source files in src 
set(DD_FILES digitdodo_platform.cpp)


include_directories(${CMAKE_SOURCE_DIR}/../../include/platform_contracts)
include_directories(${CMAKE_SOURCE_DIR}/../)


# Create the main library from src 
add_library(digitdodoplatform SHARED ${DD_FILES})

install(TARGETS digitdodoplatform
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)