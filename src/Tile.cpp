#include "Tile.hpp"

#include <iostream>
#include <string>

#include "GlobalSettings.hpp"
#include "SFML/Graphics/Color.hpp"
#include "SFML/System/Vector2.hpp"

CTile::CTile(int id, int colorId, sf::Color color, sf::Vector2i coords, sf::Vector2f pos) : m_id(id), m_colorId(colorId), m_color(color), m_coords(coords), m_pos(pos), m_mark()
{
    // Load font
    m_font.loadFromFile(std::string(GlobalSettings::FONTS_PATH) + "arial.ttf");
}

void CTile::Init()
{
    // Position
    m_tile.setPosition(m_pos);

    m_tile.setSize({static_cast<float>(GlobalSettings::TILE_SIZE), static_cast<float>(GlobalSettings::TILE_SIZE)});

    m_tile.setOutlineThickness(1.0f);
    m_tile.setOutlineColor(sf::Color::Black);

    m_tile.setFillColor(m_color);

    // Text
    m_markText.setCharacterSize(16);
    m_markText.setFillColor(sf::Color::Black);
    m_markText.setStyle(sf::Text::Regular);
    m_markText.setPosition(m_pos.x + (GlobalSettings::TILE_SIZE / 2) - 8, m_pos.y + (GlobalSettings::TILE_SIZE / 2 - 8));
}

void CTile::Draw(sf::RenderWindow& window)
{
    // Tile
    window.draw(m_tile);

    // Text
    m_markText.setFont(m_font);
    window.draw(m_markText);
}

bool CTile::MouseDetection(sf::Mouse::Button mouseButton, sf::Vector2i mousePos)
{
    sf::Vector2f mousePosFloat = sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
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

void CTile::SetId(int id)
{
    m_id = id;
}

int CTile::GetId() const
{
    return m_id;
}

void CTile::SetColorId(int colorId)
{
    m_colorId = colorId;
}

int CTile::GetColorId() const
{
    return m_colorId;
}

sf::Color CTile::GetColor() const
{
    return m_color;
}

void CTile::SetColor(const sf::Color& color)
{
    m_color = color;
}

void CTile::SetCoords(const sf::Vector2i& coords)
{
    m_coords = coords;
}

sf::Vector2i CTile::GetCoords() const
{
    return m_coords;
}

void CTile::SetPosition(const sf::Vector2f& pos)
{
    m_tile.setPosition(pos);
}

sf::Vector2f CTile::GetPosition() const
{
    return m_tile.getPosition();
}

void CTile::SetMark(const std::string &mark)
{
    m_mark = mark;
    m_markText.setString(m_mark);
}

std::string CTile::GetMark() const
{
    return m_mark;
}

sf::Vector2f CTile::GetSize() const
{
    return m_tile.getSize();
}

bool CTile::isMarkEmpty() const
{
    return m_mark.empty();
}

bool CTile::isMarkX() const
{
    return m_mark == "X";
}

bool CTile::isMarkQueen() const
{
    return m_mark == "Q";
}

void CTile::ClearMark()
{
    m_mark.clear();
    m_markText.setString(m_mark);
    m_markText.setStyle(sf::Text::Regular);
}

void CTile::PlaceX()
{
    m_mark = "X";
    m_markText.setString(m_mark);
    m_markText.setStyle(sf::Text::Regular);
}

void CTile::PlaceQueen()
{
    m_mark = "Q";
    m_markText.setString(m_mark);
    m_markText.setStyle(sf::Text::Bold);
}

// -------

std::ostream& operator<<(std::ostream& os, const CTile& tile)
{
    os << "Tile" << std::endl;
    os << "Id: " << tile.GetId() << std::endl;
    os << "Color Id: " << tile.GetColorId() << std::endl;
    os << "Color: " << tile.GetColor().toInteger() << std::endl; // TODO: toString()
    os << "Coords.: [" << tile.GetCoords().x << ", " << tile.GetCoords().y << "]" << std::endl;
    os << std::endl;

    return os;
}
