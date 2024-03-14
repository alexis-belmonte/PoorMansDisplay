#pragma once

#include "pmd/Texture.hpp"

#include <istream>
#include <string>
#include <map>

namespace PMD
{
    class TextureLoader
    {
    protected:
        typedef Texture (*System)(std::istream &);

        static const std::map<std::string, TextureLoader::System> SYSTEMS;

        static Texture fromPNG(std::istream &stream);

    public:
        static Texture fromLocalStorage(const std::string &path);
    };
};
