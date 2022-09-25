cmake_minimum_required(VERSION 3.12)
project(bumpers)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
option(EMBEDDED_ASSETS "Include assets in binary" ON)
option(WEBASSEMBLY "Configure for webassembly build" OFF)
add_definitions(-DHAVE_STDINT_H -D_XBOX -DHAVE_STAT)

if (WEBASSEMBLY)
    add_definitions(-DWEBASSEMBLY)
elseif (WIN32)
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

if (WEBASSEMBLY)
    set(ENGINE_PLATFORM_SOURCE
        engine/webassembly/wasm_display.cpp engine/display.hpp
    )

    set(PLATFORM_ASSET_LIST
        shaders/webassembly/wasm_default.glsl
        shaders/webassembly/wasm_bloom.glsl
        shaders/webassembly/wasm_skybox.glsl
        shaders/webassembly/wasm_blur.glsl
    )
else()
    set(ENGINE_PLATFORM_SOURCE
        engine/display.cpp engine/display.hpp
        engine/assets/file_asset_repository.cpp engine/assets/file_asset_repository.hpp
    )

    set(PLATFORM_ASSET_LIST
        shaders/default.glsl
        shaders/bloom.glsl
        shaders/skybox.glsl
        shaders/blur.glsl
    )

    add_subdirectory(${GLEW})
    add_subdirectory(${FREEGLUT})

    include_directories(
        ${FREEGLUT}/include
        ${GLEW}/include
    )
endif()

add_subdirectory(${PUGIXML})
include_directories(
    ${GLM} 
    ${STB}
    ${PUGIXML}/src
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(ENGINE_SOURCES
    ${ENGINE_PLATFORM_SOURCE}
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
    engine/input.cpp engine/input.hpp
    engine/assets/asset_repository.hpp
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

SET(ASSET_LIST
    ${PLATFORM_ASSET_LIST}
    models/bumper.dae
    models/arena.dae
    textures/bumper/PaintWork.jpg
    textures/bumper/NormalMap.jpg
    textures/bumper/cart_ambient_occlusion.jpg
    textures/bumper/white.jpg
    textures/bumper/metal_normal.jpg
    textures/bumper/cart_floor_ambient_occlusion.jpg
    textures/metal/diamond-metal-siding_albedo.jpg
    textures/metal/worn-shiny-metal-albedo.jpg
    textures/metal/steelplate1_normal-ogl.jpg
    textures/metal/steelplate1_albedo.jpg
    textures/metal/worn-shiny-metal-Normal-ogl.jpg
    textures/metal/diamond-metal-siding_normal-ogl.jpg
    textures/wood/wood.jpg
    textures/wood/wood_normal.jpg
    textures/roof/ledge_lightmap.jpg
    textures/roof/roof.jpg
    textures/roof/support_normal.jpg
    textures/roof/Grass001_2K_NormalGL.jpg
    textures/roof/support.jpg
    textures/roof/support_lightmap.jpg
    textures/roof/Asphalt007_2K_NormalGL.jpg
    textures/roof/floor_lightmap.jpg
    textures/roof/agedplanks1-normal4-unity.jpg
    textures/roof/agedplanks1-albedo.jpg
    textures/roof/roof_lightmap.jpg
    textures/roof/Grass001_2K_Color.jpg
    textures/roof/Asphalt007_2K_Color.jpg
    textures/skybox/skybox_negative_y.jpg
    textures/skybox/skybox_negative_x.jpg
    textures/skybox/skybox_positive_y.jpg
    textures/skybox/skybox_negative_z.jpg
    textures/skybox/skybox_positive_x.jpg
    textures/skybox/skybox_positive_z.jpg
)

if (EMBEDDED_ASSETS)
    foreach(x ${ASSET_LIST})
        configure_file(assets/${x} assets/${x} COPYONLY)
    endforeach()
    configure_file(tools/asset_generator.py tools/asset_generator.py COPYONLY)

    message(STATUS "Generating embedded assets")
    execute_process(COMMAND
        ${CMAKE_CURRENT_SOURCE_DIR}/tools/asset_generator.py
            --output-dir ${CMAKE_BINARY_DIR}
            --asset-dir ${CMAKE_CURRENT_SOURCE_DIR}/assets
            ${ASSET_LIST}
    )

    include(${CMAKE_BINARY_DIR}/assets/cmake_file_list.txt)
    include_directories(${CMAKE_BINARY_DIR})
endif()

SET(SOURCES
    ${ENGINE_SOURCES}
    ${GAMEOBJECT_SOURCES}
    ${GAME_SOURCES}
    ${ASSET_SOURCES}
    main.cpp
)

source_group("engine" FILES ${ENGINE_SOURCES})
source_group("gameobject" FILES ${GAMEOBJECT_SOURCES})
source_group("game" FILES ${GAME_SOURCES})

add_executable(bumpers ${SOURCES})

if (WEBASSEMBLY)
    target_link_libraries(bumpers pugixml-static)
    target_link_options(bumpers PRIVATE -sEXPORTED_FUNCTIONS=['_main','_resize_display'])
    target_link_options(bumpers PRIVATE -sEXPORTED_RUNTIME_METHODS=['ccall','cwrap'])
    target_link_options(bumpers PRIVATE -sTOTAL_MEMORY=1310720000)
    target_link_options(bumpers PRIVATE -sGL_PREINITIALIZED_CONTEXT)
elseif (WIN32)
    target_link_libraries(bumpers libglew_static freeglut_static pugixml-static)
else()
    target_link_libraries(bumpers libglew_static freeglut_static pugixml-static pthread)
endif()

