/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <iostream>
#include <memory>
#include <string_view>

namespace Engine {

class AssetRepository {
public:
    virtual ~AssetRepository() = default;
    virtual std::unique_ptr<std::istream> open(std::string_view name) const = 0;
    virtual std::shared_ptr<AssetRepository> copy() const = 0;

protected:
    AssetRepository() = default;
};

}
