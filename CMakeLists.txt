cmake_minimum_required(VERSION 3.25)
project(ld001 DESCRIPTION "basic land based robot")

include(FindPkgConfig)
include(FetchContent)

set(CPACK_GENERATOR "DEB")    
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CPACK_PACKAGE_NAME "ld001")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "1")

set(CPACK_SOURCE_IGNORE_FILES "/gtest/;/googletest/")
set(CPACK_PACKAGE_CONTACT "Aaron Spiteri")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/resources/scripts/postinst")
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/resources/scripts/prerm")

include(CPack)

set(VERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH})

pkg_search_module(FATCNT REQUIRED rrobot)
pkg_search_module(DLIB REQUIRED dlib-1)

set(EXTRA_LIBS "wiringPi")

# Sets up variables that can be used to configure robot.
set(MANIFEST_MULTITYPE "LANDDRONE_2W")
set(MANIFEST_AI_QUEUE_FC "MICROCONTROLLER")
set(MANIFEST_AI_QUEUE_MC "MICROCONTROLLER")
configure_file(resources/manifest.json.in resources/manifest.json @ONLY)
configure_file(resources/fatcnt.env.in resources/fatcnt.env @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/resources/fatcnt.env"
  DESTINATION /opt/fatcnt/etc/
  PERMISSIONS OWNER_READ OWNER_WRITE  GROUP_READ  WORLD_READ
)

# This is done at an application level
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/resources/scripts/fatcnt.service"
        DESTINATION /etc/systemd/system
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/resources/manifest.json"
        DESTINATION /opt/fatcnt/etc/
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/fatcnt" DESTINATION 
  /opt/fatcnt/bin 
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

set(fatcnt_SOURCES
    src/ld001.cpp
    src/ld001/events/msp104/msp104_ctl_in.cpp
    src/ld001/events/msp104/msp104_hbridge_serializer.cpp
    src/ld001/events/ai/ai_virtual_predictor.cpp
    src/ld001/events/hbridgecontroller.cpp
    src/ld001/events/catagorizer/ldsqu001mapper.cpp
)

set(JSON_VERSION v3.11.3)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/${JSON_VERSION}/json.tar.xz)
FetchContent_MakeAvailable(json)
list(APPEND CMAKE_MODULE_PATH ${nlohmann_json})

include_directories(${json_SOURCE_DIR}/include src ${LIBSERIAL_INCLUDE_DIRS} ${PROJECT_BINARY_DIR}/include)

add_executable(fatcnt ${fatcnt_SOURCES})
target_link_libraries(fatcnt rrobot dlib wiringPi)