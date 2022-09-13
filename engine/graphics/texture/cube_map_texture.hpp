/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "texture.hpp"
#include "engine/forward.hpp"
#include <string>
#include <vector>

namespace Engine
{

    class CubeMapTexture : public Texture
    {
    public:
		static std::shared_ptr<CubeMapTexture> construct(const AssetRepository&, std::string_view name_prefix);

        virtual void bind(int slot) const final;

    private:
        CubeMapTexture(int texture)
            : Texture(texture)
        {
        }

        mutable std::vector<std::tuple<uint8_t*, int, int>> m_data;

    };

}
