cmake_minimum_required(VERSION 3.10)
project(Asteroids)

set(CMAKE_CXX_STANDARD 11)

configure_file(images/background.jpg images/background.jpg COPYONLY)

configure_file(images/fire_red.png images/fire_red.png COPYONLY)
configure_file(images/fire_orange.png images/fire_orange.png COPYONLY)
configure_file(images/fire_yellow.png images/fire_yellow.png COPYONLY)
configure_file(images/fire_green.png images/fire_green.png COPYONLY)
configure_file(images/fire_blue.png images/fire_blue.png COPYONLY)
configure_file(images/fire_purple.png images/fire_purple.png COPYONLY)

configure_file(images/rock.png images/rock.png COPYONLY)
configure_file(images/rock_small.png images/rock_small.png COPYONLY)
configure_file(images/spaceship.png images/spaceship.png COPYONLY)
configure_file(images/explosions/type_B.png images/explosions/type_B.png COPYONLY)
configure_file(images/explosions/type_C.png images/explosions/type_C.png COPYONLY)
configure_file(images/UFO128pix.png images/UFO128pix.png COPYONLY)
configure_file(sounds/UFOalarm.ogg sounds/UFOalarm.ogg COPYONLY)
configure_file(sounds/explosionSound.ogg sounds/explosionSound.ogg COPYONLY)
configure_file(fonts/Orbitron-Regular.ttf fonts/Orbitron-Regular.ttf COPYONLY)
configure_file(fonts/Orbitron-Medium.ttf fonts/Orbitron-Medium.ttf COPYONLY)
configure_file(fonts/Orbitron-Bold.ttf fonts/Orbitron-Bold.ttf COPYONLY)


#file(COPY images DESTINATION images)

# Define sources and executable
set(EXECUTABLE_NAME "Asteroids")

add_executable(${EXECUTABLE_NAME} main.cpp)

# Detect and add SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
#Find any version 2.X of SFML
#See the FindSFML.cmake file for additional details and instructions
find_package(SFML 2 REQUIRED network audio graphics window system)
if(SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
endif()