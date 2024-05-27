/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "component.hpp"
#include <set>
#include <string>

namespace Object {

class Attributes : public ComponentBase<Attributes> {
    friend ComponentBase<Attributes>;

public:
    [[nodiscard]] inline bool has(std::string const& attr) const { return m_attributes.contains(attr); }

private:
    template<typename... Args>
    explicit Attributes(Args... args)
        : m_attributes { args... }
    {
    }

    Attributes(Attributes const&) = default;

    std::set<std::string> m_attributes;
};

}
