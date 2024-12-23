#include "Tile.hpp"

#include <algorithm>
#include <iostream>

#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/System/Vector2.hpp"

#include "GlobalSettings.hpp"
#include "ResourceManager.hpp"

Tile::Tile(int id, int colorId, sf::Color color, sf::Vector2i coords, sf::Vector2f pos) : m_id(id), m_colorId(colorId), m_originalColor(color), m_coords(coords), m_pos(pos), m_mark(Mark::EMPTY)
{
    // Set the original color as the current color
    m_currentColor = m_originalColor;
}

void Tile::Init()
{
    // Position
    m_tile.setPosition(m_pos);

    // Size
    m_tile.setSize({static_cast<float>(GlobalSettings::TILE_SIZE), static_cast<float>(GlobalSettings::TILE_SIZE)});

    // Outline
    m_tile.setOutlineThickness(1.0f);
    m_tile.setOutlineColor(sf::Color::Black);

    // Color
    m_tile.setFillColor(m_currentColor);

    // Sprite
    ClearMark();
}

void Tile::Draw(sf::RenderWindow& window)
{
    // Tile
    window.draw(m_tile);

    // Sprite (on top)
    window.draw(m_sprite);
}

bool Tile::MouseHover(sf::Vector2i mousePos)
{
    const sf::Vector2f mousePosFloat = sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
    if (m_tile.getGlobalBounds().contains(mousePosFloat) && !m_isBeingHovered)
    {
        DarkenColor();
        m_isBeingHovered = true;

        return true;
    }
    else if (!m_tile.getGlobalBounds().contains(mousePosFloat) && m_isBeingHovered)
    {
        ResetColor();
        m_isBeingHovered = false;

        return false;
    }

    return false;
}

void Tile::ResetHoverState()
{
    m_isBeingHovered = false;
}

void Tile::DarkenColor()
{
    const sf::Color darkerColor = sf::Color(
        std::max(0, static_cast<int>(m_originalColor.r) - GlobalSettings::TILE_DARK_FACTOR),
        std::max(0, static_cast<int>(m_originalColor.g) - GlobalSettings::TILE_DARK_FACTOR),
        std::max(0, static_cast<int>(m_originalColor.b) - GlobalSettings::TILE_DARK_FACTOR)
    );
    UpdateColor(darkerColor);
}

bool Tile::MouseDetection(sf::Mouse::Button mouseButton, sf::Vector2i mousePos)
{
    const sf::Vector2f mousePosFloat = sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
    if (m_tile.getGlobalBounds().contains(mousePosFloat))
    {
        switch (mouseButton)
        {
        case sf::Mouse::Button::Left:
        {
            if (isMarkEmpty())
            {
                PlaceX();
            }
            else if (isMarkX())
            {
                PlaceQueen();
            }
            else if (isMarkQueen())
            {
                ClearMark();
            }

            return true;
        }
        case sf::Mouse::Button::Right:
        default:
            break;
        }
    }

    return false;
}

void Tile::UpdateColor(sf::Color color)
{
    m_currentColor = color;
    m_tile.setFillColor(m_currentColor);
}

void Tile::ResetColor()
{
    m_currentColor = m_originalColor;
    m_tile.setFillColor(m_currentColor);
}

sf::Vector2f Tile::GetSize() const
{
    return m_tile.getSize();
}

sf::FloatRect Tile::GetGlobalBounds() const
{
    return m_tile.getGlobalBounds();
}

bool Tile::isMarkEmpty() const
{
    return m_mark == Mark::EMPTY;
}

bool Tile::isMarkX() const
{
    return m_mark == Mark::X;
}

bool Tile::isMarkQueen() const
{
    return m_mark == Mark::QUEEN;
}

void Tile::ClearMark()
{
    m_mark = Mark::EMPTY;

    const sf::Texture& transparentTexture = ResourceManager::getInstance().getTransparentTexture();

    SetSprite(transparentTexture);
}

void Tile::PlaceX()
{
    m_mark = Mark::X;

    const sf::Texture& xTexture = ResourceManager::getInstance().getXTexture();
    const float offset = (GlobalSettings::TILE_SIZE - (GlobalSettings::TILE_SIZE / 4.0)) / 2.0;
    const sf::Vector2f scale = {0.25f, 0.25f};

    SetSprite(xTexture, offset, scale);
}

void Tile::PlaceQueen()
{
    m_mark = Mark::QUEEN;

    const sf::Texture& queenTexture = ResourceManager::getInstance().getQueenTexture();
    const float offset = GlobalSettings::TILE_SIZE / 4.0;
    const sf::Vector2f scale = {0.5f, 0.5f};

    SetSprite(queenTexture, offset, scale);
}

void Tile::SetSprite(const sf::Texture& texture, const float offset, const sf::Vector2f& scale)
{
    m_sprite.setTexture(texture);
    m_sprite.setPosition(m_pos.x + offset, m_pos.y + offset);
    m_sprite.setScale(scale);
}

// -------

std::ostream& operator<<(std::ostream& os, const Tile& tile)
{
    os << "Tile" << std::endl;
    os << "Id: " << tile.GetId() << std::endl;
    os << "Color Id: " << tile.GetColorId() << std::endl;
    os << "Original Color: " << tile.GetOriginalColor().toInteger() << std::endl;
    os << "Current Color: " << tile.GetCurrentColor().toInteger() << std::endl;
    os << "Coords.: [" << tile.GetCoords().x << ", " << tile.GetCoords().y << "]" << std::endl;
    os << std::endl;

    return os;
}
