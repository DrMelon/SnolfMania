cmake_minimum_required(VERSION 3.7)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY mods/SnolfMania)

project(SnolfMania)
set(MOD_NAME SnolfMania CACHE STRING "The mod directory to look into")

set(OUTPUT_NAME "SnolfMania" CACHE STRING "The name of the built library")

add_library(SnolfMania SHARED 
    GameAPI/C/GameAPI/Game.c
    ${MOD_NAME}/dllmain.c
    ${MOD_NAME}/Objects/Player.c
    ${MOD_NAME}/Objects/Snolf.c
    ${MOD_NAME}/Objects/Starpost.c
    ${MOD_NAME}/Objects/HUD.c
    ${MOD_NAME}/Objects/Zone.c
    ${MOD_NAME}/Objects/DashLift.c
)

target_include_directories(SnolfMania PRIVATE
    ${MOD_NAME}/
    ${MOD_NAME}/Objects/
    GameAPI/C/
)

set_target_properties(SnolfMania PROPERTIES OUTPUT_NAME ${OUTPUT_NAME})

unset(MOD_NAME CACHE)
unset(OUTPUT_NAME CACHE)
