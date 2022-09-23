#!/usr/bin/env python

import sys
import os
import json
import hashlib
from argparse import ArgumentParser
from typing import TextIO
from pathlib import Path

HEADER = """
#pragma once

#include "engine/assets/asset_repository.hpp"

namespace Game
{

class EmbeddedAssetRepository final : public Engine::AssetRepository
{
public:
    static EmbeddedAssetRepository construct();
    std::unique_ptr<std::istream> open(std::string_view name) const override;
    std::shared_ptr<AssetRepository> copy() const override;

private:
    explicit EmbeddedAssetRepository() = default;

};

}

"""

IMPLEMENTATION = """
using namespace Game;

class MemoryBuffer : public std::basic_streambuf<char>
{
public:
    MemoryBuffer(const uint8_t *buffer, size_t length)
    {
        setg((char*)buffer, (char*)buffer, (char*)buffer + length);
    }

private:
    virtual std::streampos seekoff(std::streamoff offset, std::ios_base::seekdir way, std::ios_base::openmode) override
    {
        switch (way)
        {
            case std::ios_base::beg:
                setg(eback(), eback() + offset, egptr());
                return offset;
            case std::ios_base::cur:
                setg(eback(), gptr() + offset, egptr());
                return static_cast<std::streampos>(gptr() - eback()) + offset;
            case std::ios_base::end:
                setg(eback(), egptr() + offset, egptr());
                return static_cast<std::streampos>(egptr() - eback()) + offset;
            default:
                return -1;
        }
    }

    virtual std::streampos seekpos(std::streampos position, std::ios_base::openmode) override
    {
        setg(eback(), eback() + position, egptr());
        return position;
    }

};

class MemoryStream : public std::istream
{
public:
    MemoryStream(const uint8_t *buffer, size_t length)
        : std::istream(&m_buffer)
        , m_buffer(buffer, length)
    {
        rdbuf(&m_buffer);
    }

private:
    MemoryBuffer m_buffer;

};

EmbeddedAssetRepository EmbeddedAssetRepository::construct()
{
    return EmbeddedAssetRepository();
}

std::unique_ptr<std::istream> EmbeddedAssetRepository::open(std::string_view name) const
{
    auto [buffer, length] = lookup_embedded_asset_from_name(name);
    if (buffer == nullptr)
    {
        std::cerr << "Error: Unknown asset '" << name << "'\\n";
        return nullptr;
    }

    auto stream = std::make_unique<MemoryStream>(buffer, length);
    return std::move(stream);
}

std::shared_ptr<Engine::AssetRepository> EmbeddedAssetRepository::copy() const
{
    return std::shared_ptr<EmbeddedAssetRepository>(new EmbeddedAssetRepository());
}

"""

def normalize_name(name: str) -> str:
    normalized = ''
    for char in name:
        lower = char.lower()
        if lower >= 'a' and lower <= 'z':
            normalized += char
        elif lower >= '0' and lower <= '9':
            normalized += char
        else:
            normalized += '_'
    return normalized

def generate_asset_implementations(output_dir: Path, asset_dir: Path, asset_list: list[str]):
    new_asset_hashes: dict[str, str] = {}
    old_asset_hashes: dict[str, str] = {}

    hashes_path = output_dir.joinpath('assets/hashes.json')
    if hashes_path.exists():
        with open(hashes_path, 'r', encoding='UTF-8') as file:
            old_asset_hashes = json.loads(file.read())

    for path in asset_list:
        name = normalize_name(path)
        with open(asset_dir.joinpath(path), 'rb') as file:
            content = file.read()
            hasher = hashlib.sha1()
            hasher.update(content)
            new_asset_hashes[path] = hasher.hexdigest()
            if new_asset_hashes.get(path) == old_asset_hashes.get(path):
                continue

            print(f' -> { path }')
            with open(output_dir.joinpath(f'assets/{ name }.cpp'), 'w') as out:
                out.write(f'const unsigned char *{ name } = (unsigned char[]){{ ')
                for byte in content:
                    out.write(f'{ byte }, ')
                out.write('};\n')

    with open(hashes_path, 'w') as file:
        file.write(json.dumps(new_asset_hashes))

def generate_cmake_file_list(output_dir: Path, asset_list: list[str]):
    with open(output_dir.joinpath(f'assets/cmake_file_list.txt'), 'w') as out:
        out.write('set(ASSET_SOURCES')
        for path in asset_list:
            out.write(f' { output_dir.joinpath("assets/" + normalize_name(path) + ".cpp") }')
        out.write(f' { output_dir.joinpath("embedded_assets.cpp") }')
        out.write(')\n')

def generate_asset_definitions(out: TextIO, asset_dir: Path, asset_list: list[str]):
    out.write('\n')
    for path in asset_list:
        name = normalize_name(path)
        with open(asset_dir.joinpath(path), 'rb') as file:
            size = len(file.read())
        out.write(f'const unsigned int { name }_size = { size };\n')
        out.write(f'extern const unsigned char *{ name };\n')
    out.write('\n')

def generate_lookup_function(out: TextIO, asset_list: list[str]):
    out.write('\n')
    out.write('static std::tuple<const uint8_t*, size_t> lookup_embedded_asset_from_name(std::string_view name)\n')
    out.write('{\n')
    for path in asset_list:
        name = normalize_name(path)
        out.write(f'    if (name == "/{ path }") return {{ { name }, { name }_size }};\n')
    out.write('    return { nullptr, 0 };\n')
    out.write('}\n')

def main():
    parser = ArgumentParser(description='Generate embedded assets')
    parser.add_argument('--output-dir', type=str, required=True, help='Directory to generate source into')
    parser.add_argument('--asset-dir', type=str, required=True, help='Directory containing the assets')
    parser.add_argument('assets', type=str, nargs='*', help='List of assets to generate from')
    options = parser.parse_args(sys.argv[1:])
    output_dir = Path(options.output_dir)
    asset_dir = Path(options.asset_dir)

    os.makedirs(output_dir, exist_ok=True)
    os.makedirs(output_dir.joinpath('assets'), exist_ok=True)

    with open(output_dir.joinpath('embedded_assets.hpp'), 'w') as out:
        out.write(HEADER)

    with open(output_dir.joinpath('embedded_assets.cpp'), 'w') as out:
        out.write(f'\n#include "embedded_assets.hpp"\n')
        out.write('#include <iostream>\n')
        out.write('#include <memory>\n')
        out.write('#include <string_view>\n')
        generate_asset_definitions(out, asset_dir, options.assets)
        generate_lookup_function(out, options.assets)
        out.write(IMPLEMENTATION)

    generate_asset_implementations(output_dir, asset_dir, options.assets)
    generate_cmake_file_list(output_dir, options.assets)

if __name__ == '__main__':
    main()

