#ifndef TILE_HPP
#define TILE_HPP

#include <SFML/Graphics.hpp>

#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/RectangleShape.hpp"

class CTile
{
public:
    CTile(int id, int colorId, sf::Color color, sf::Vector2i coords, sf::Vector2f pos);

    ~CTile() = default;

    void Init();

    void Draw(sf::RenderWindow& window);
    bool MouseDetection(sf::Mouse::Button mouseButton, sf::Vector2i mousePos);

    void SetId(int id);
    int GetId() const;

    void SetColorId(int colorId);
    int GetColorId() const;

    void SetColor(const sf::Color& color);
    sf::Color GetColor() const;

    void SetCoords(const sf::Vector2i& coords);
    sf::Vector2i GetCoords() const;

    void SetPosition(const sf::Vector2f& pos);
    sf::Vector2f GetPosition() const;

    void SetMark(const std::string& mark);
    std::string GetMark() const;

    sf::Vector2f GetSize() const;

    bool isMarkEmpty() const;
    bool isMarkX() const;
    bool isMarkQueen() const;

    void ClearMark();
    void PlaceX();
    void PlaceQueen();

private:
    sf::RectangleShape m_tile;

    sf::Font m_font;
    sf::Text m_markText;

    int m_id;
    int m_colorId;
    sf::Color m_color;
    sf::Vector2i m_coords;
    sf::Vector2f m_pos;
    std::string m_mark;

    // -------

    friend std::ostream& operator<<(std::ostream& os, const CTile& tile);
};

#endif // TILE_HPP
