/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "component.hpp"
#include <string>
#include <set>

namespace Object
{

	class Attributes : public ComponentBase<Attributes>
	{
		friend ComponentBase<Attributes>;

	public:
		inline bool has(const std::string& attr) const { return m_attributes.contains(attr); }

	private:
		template<typename... Args>
		Attributes(Args... args)
			: m_attributes { args... }
		{
		}

		Attributes(const Attributes&) = default;

		std::set<std::string> m_attributes;

	};

}
