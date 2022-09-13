/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "texture.hpp"
#include "engine/forward.hpp"
#include <string>

namespace Engine
{

    class ImageTexture : public Texture
    {
    public:
		static std::shared_ptr<ImageTexture> construct(const AssetRepository&, std::string_view name);

        virtual void bind(int slot) const final;

    private:
        ImageTexture(int texture)
            : Texture(texture)
        {
        }

        std::string m_name;

        mutable uint8_t *m_data { nullptr };
        int m_width { 0 };
        int m_height { 0 };

    };

}

