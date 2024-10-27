#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include <iostream>
#include <string>

#include "SFML/Graphics/Texture.hpp"

#include "GlobalSettings.hpp"

class ResourceManager
{
public:
    // Static member function to get the instance
    static ResourceManager& getInstance()
    {
        static ResourceManager instance; // Guaranteed to be initialized only once
        return instance;
    }

    void Load() // example member function
    {
        std::cout << "Loading resources..." << std::endl;

        // Load textures
        const std::string texturesPath = std::string(GlobalSettings::TEXTURES_PATH);

        const std::string xTexturePath = texturesPath + "x_icon.png";
        if (m_xTexture.loadFromFile(xTexturePath))
        {
            std::cout << "Texture: " << xTexturePath << " loaded correctly" << std::endl;
        }
        const std::string queenTexturePath = texturesPath + "queen_icon.png";
        if (m_queenTexture.loadFromFile(queenTexturePath))
        {
            std::cout << "Texture: " << queenTexturePath << " loaded correctly" << std::endl;
        }
        const std::string transparentTexturePath = texturesPath + "transparent_icon.png";
        if (m_transparentTexture.loadFromFile(transparentTexturePath))
        {
            std::cout << "Texture: " << transparentTexturePath << " loaded correctly" << std::endl;
        }
    }

    // By using const sf::Texture&, we ensure that you we referencing the original texture managed by the ResourceManager rather than creating a copy
    const sf::Texture& getXTexture() const { return m_xTexture; }
    const sf::Texture& getQueenTexture() const { return m_queenTexture; }
    const sf::Texture& getTransparentTexture() const { return m_transparentTexture; }

    ResourceManager(const ResourceManager&) = delete; // Delete copy constructor
    ResourceManager& operator =(const ResourceManager&) = delete; // Delete copy assignment operator

private:
    ResourceManager() = default; // Private constructor to prevent instantation
    ~ResourceManager() = default; // Default destructor

    // Textures
    sf::Texture m_xTexture;
    sf::Texture m_queenTexture;
    sf::Texture m_transparentTexture;
};

#endif // RESOURCE_MANAGER_HPP
