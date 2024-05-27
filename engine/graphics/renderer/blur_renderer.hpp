/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "engine/forward.hpp"
#include "post_process_renderer.hpp"

namespace Engine {

class BlurRenderer : public PostProcessRenderer {
public:
    BlurRenderer(std::shared_ptr<Shader> shader,
        std::shared_ptr<Texture> view, glm::vec2 direction);

    ~BlurRenderer() override;

    void resize_view(int width, int height);

protected:
    void bind_inputs() final;
    void on_resize(int width, int height) final;

    std::shared_ptr<Texture> m_view;
    glm::vec2 m_direction;
    glm::vec2 m_view_scale { 1, 1 };
};

}
