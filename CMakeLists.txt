cmake_minimum_required(VERSION 3.12)
project(bumpers)

set(CMAKE_BUILD_TYPE Release)
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
set(FREEGLUT_BUILD_DEMOS OFF CACHE BOOL "Build FreeGLUT demos." FORCE)
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
    engine/physics/collision_shape_2d.cpp engine/physics/collision_shape_2d.hpp
    engine/physics/collision_shape_utils_2d.cpp engine/physics/collision_shape_utils_2d.hpp
    engine/physics/collision_resolver_2d.cpp engine/physics/collision_resolver_2d.hpp
    engine/physics/broad_phase_collision_2d.cpp engine/physics/broad_phase_collision_2d.hpp
    engine/assets/collada_loader.cpp engine/assets/collada_loader.hpp
    engine/assets/thread_pool.cpp engine/assets/thread_pool.hpp
    engine/assets/file_asset_repository.cpp engine/assets/file_asset_repository.hpp
    engine/assets/asset_repository.hpp
    engine/input.cpp engine/input.hpp
    engine/display.cpp engine/display.hpp
    engine/logger.cpp engine/logger.hpp
    engine/forward.hpp
)

SET(GAMEOBJECT_SOURCES
    gameobject/component.hpp
    gameobject/mesh_render.hpp
    gameobject/light.hpp
    gameobject/camera.hpp
    gameobject/attributes.hpp
    gameobject/transform.cpp gameobject/transform.hpp
    gameobject/gameobject.cpp gameobject/gameobject.hpp
    gameobject/physics/physics_body_2d.cpp gameobject/physics/physics_body_2d.hpp
    gameobject/physics/collider_2d.cpp gameobject/physics/collider_2d.hpp 
    gameobject/physics/box_bounds_3d.cpp gameobject/physics/box_bounds_3d.hpp 
    gameobject/world.hpp
    gameobject/scene.hpp
    gameobject/forward.hpp
)

SET(GAME_SOURCES
    game/in_car_camera.cpp game/in_car_camera.hpp
    game/look_at_camera.cpp game/look_at_camera.hpp
    game/free_camera.cpp game/free_camera.hpp
    game/bumper_cars_scene.cpp game/bumper_cars_scene.hpp
    game/car_engine.cpp game/car_engine.hpp
    game/player_controller.cpp game/player_controller.hpp
    game/ai.cpp game/ai.hpp
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

if (WIN32)
    target_link_libraries(bumpers libglew_static freeglut_static pugixml-static)
else()
    target_link_libraries(bumpers libglew_static freeglut_static pugixml-static pthread)
endif()
