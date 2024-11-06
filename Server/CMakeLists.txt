#Networking
include_directories(${CMAKE_SOURCE_DIR}/Networking/include)
include_directories(${CMAKE_SOURCE_DIR}/Networking/rapidjson/include)

# Link libraries
target_link_libraries(DurakClient durak_networking)