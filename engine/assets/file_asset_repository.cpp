/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "file_asset_repository.hpp"
#include <utility>
#include <filesystem>
#include <fstream>
#include <cstring>

using namespace Engine;

FileAssetRepository FileAssetRepository::construct(std::string directory)
{
    return { std::move(directory) };
}

std::unique_ptr<std::istream> FileAssetRepository::open(std::string_view name) const
{
    std::filesystem::path path;
    path += m_directory;
    path += name;

    auto stream = std::make_unique<std::ifstream>(path);
    if (!stream->good())
    {
        std::cerr << "Error opening file '" << name << "': " << strerror(errno) << "\n";
        return {};
    }

    return std::move(stream);
}

std::shared_ptr<AssetRepository> FileAssetRepository::copy() const
{
    return std::shared_ptr<FileAssetRepository>(new FileAssetRepository(m_directory));
}

FileAssetRepository::FileAssetRepository(std::string directory)
    : m_directory(std::move(directory))
{
}
