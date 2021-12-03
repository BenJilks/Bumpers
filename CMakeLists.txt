cmake_minimum_required(VERSION 3.12)
project(bumpers)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-DHAVE_STDINT_H -D_XBOX -DHAVE_STAT)

if (WIN32)
    add_definitions(-DHAVE_SLEEP -DHAVE_WINDOWS_H)
else()
    add_definitions(-DHAVE_NANOSLEEP -DHAVE_TIME_H)
endif()

set(GLEW 3rdparty/glew)
set(FREEGLUT 3rdparty/FreeGLUT/freeglut/freeglut)
set(GLM 3rdparty/glm)
set(STB 3rdparty/stb)
set(PUGIXML 3rdparty/pugixml)

set(LIBTYPE STATIC)
add_subdirectory(${GLEW})
add_subdirectory(${FREEGLUT})
add_subdirectory(${PUGIXML})

include_directories(
    ${FREEGLUT}/include
    ${GLEW}/include 
    ${GLM} 
    ${STB}
    ${PUGIXML}/src
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(ENGINE_SOURCES
    engine/graphics/shader.cpp engine/graphics/shader.hpp
    engine/graphics/mesh/mesh_builder.cpp engine/graphics/mesh/mesh_builder.hpp
    engine/graphics/mesh/mesh.cpp engine/graphics/mesh/mesh.hpp
    engine/graphics/mesh/material.hpp
    engine/graphics/renderer/renderer.cpp engine/graphics/renderer/renderer.hpp
    engine/graphics/renderer/standard_renderer.cpp engine/graphics/renderer/standard_renderer.hpp
    engine/graphics/renderer/post_process_renderer.cpp engine/graphics/renderer/post_process_renderer.hpp
    engine/graphics/renderer/bloom_renderer.cpp engine/graphics/renderer/bloom_renderer.hpp
    engine/graphics/renderer/blur_renderer.cpp engine/graphics/renderer/blur_renderer.hpp
    engine/graphics/renderer/sky_box_renderer.cpp engine/graphics/renderer/sky_box_renderer.hpp
    engine/graphics/texture/texture.cpp engine/graphics/texture/texture.hpp
    engine/graphics/texture/image_texture.cpp engine/graphics/texture/image_texture.hpp
    engine/graphics/texture/render_texture.cpp engine/graphics/texture/render_texture.hpp
    engine/graphics/texture/cube_map_texture.cpp engine/graphics/texture/cube_map_texture.hpp
    engine/graphics/gui/gui_renderer.cpp engine/graphics/gui/gui_renderer.hpp
    engine/physics/collision_shape.cpp engine/physics/collision_shape.hpp
    engine/physics/collision_shape_utils.cpp engine/physics/collision_shape_utils.hpp
    engine/physics/collision_resolver.cpp engine/physics/collision_resolver.hpp
    engine/physics/broad_phase_collision.cpp engine/physics/broad_phase_collision.hpp
    engine/assets/obj_loader.cpp engine/assets/obj_loader.hpp
    engine/assets/mtl_loader.cpp engine/assets/mtl_loader.hpp
    engine/assets/obj_like_utils.hpp
    engine/assets/collada_loader.cpp engine/assets/collada_loader.hpp
    engine/input.cpp engine/input.hpp
    engine/display.cpp engine/display.hpp
    engine/logger.cpp engine/logger.hpp
    engine/forward.hpp
)

SET(GAMEOBJECT_SOURCES
    gameobject/component.hpp
    gameobject/transform.cpp gameobject/transform.hpp
    gameobject/mesh_render.hpp
    gameobject/light.hpp
    gameobject/camera.hpp
    gameobject/attributes.hpp
    gameobject/gameobject.cpp gameobject/gameobject.hpp
    gameobject/world.cpp gameobject/world.hpp
    gameobject/physics/physics_body.cpp gameobject/physics/physics_body.hpp
    gameobject/physics/collider.cpp gameobject/physics/collider.hpp 
    gameobject/gui/control.hpp
    gameobject/gui/button.cpp gameobject/gui/button.hpp
    gameobject/scene.hpp
    gameobject/forward.hpp
)

SET(GAME_SOURCES
    game/spin.cpp game/spin.hpp
    game/debug_camera_movement.cpp game/debug_camera_movement.hpp
    game/debug_scene.cpp game/debug_scene.hpp
    game/car_engine.cpp game/car_engine.hpp
    game/player_controller.cpp game/player_controller.hpp
)

SET(SOURCES
    ${ENGINE_SOURCES}
    ${GAMEOBJECT_SOURCES}
    ${GAME_SOURCES}
    main.cpp
)

source_group("engine" FILES ${ENGINE_SOURCES})
source_group("gameobject" FILES ${GAMEOBJECT_SOURCES})
source_group("game" FILES ${GAME_SOURCES})

add_executable(bumpers ${SOURCES})
target_link_libraries(bumpers libglew_static freeglut_static pugixml-static)

