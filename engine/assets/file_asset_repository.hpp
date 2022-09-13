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
        [[nodiscard]] std::unique_ptr<std::istream> open(std::string_view name) const override;
        [[nodiscard]] std::shared_ptr<AssetRepository> copy() const override;

    private:
        explicit FileAssetRepository(std::string directory);

        std::string m_directory;

    };

}
