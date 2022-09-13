/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "asset_repository.hpp"

namespace Engine
{

    class FileAssetRepository final : public AssetRepository
    {
    public:
        static FileAssetRepository construct(std::string directory);
        virtual std::unique_ptr<std::istream> open(std::string_view name) const;
        virtual std::shared_ptr<AssetRepository> copy() const;

    private:
        FileAssetRepository(std::string directory);

        std::string m_directory;

    };

}
