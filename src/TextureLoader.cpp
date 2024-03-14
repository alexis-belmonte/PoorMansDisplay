#include "pmd/TextureLoader.hpp"

#include <filesystem>
#include <stdexcept>
#include <format>
#include <fstream>

namespace PMD
{
    const std::map<std::string, TextureLoader::System> TextureLoader::SYSTEMS = {
        { ".png", &TextureLoader::fromPNG }
    };

    Texture TextureLoader::fromLocalStorage(const std::string &path)
    {
        if (!std::filesystem::is_regular_file(path))
            throw std::runtime_error(std::format("File '{}' not found", path));

        std::filesystem::path filePath(path);
        std::string           fileExtension(filePath.extension());

        std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(),
            [](unsigned char c) { return std::tolower(c); });

        if (TextureLoader::SYSTEMS.contains(fileExtension)) {
            std::ifstream fileStream(path, std::ios::binary);
            fileStream.exceptions(std::ios::failbit);
            return TextureLoader::SYSTEMS.at(fileExtension)(fileStream);
        }
        
        throw std::runtime_error(std::format("No loader available for file extension '{}'", fileExtension));
    }
};
