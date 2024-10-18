#ifndef TILE_HPP
#define TILE_HPP

#include <SFML/Graphics.hpp>

#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Sprite.hpp"

class CTile
{
public:
    CTile(int id, int colorId, sf::Color color, sf::Vector2i coords, sf::Vector2f pos);

    ~CTile() = default;

    void Init();

    void Draw(sf::RenderWindow& window);

    bool MouseHover(sf::Vector2i mousePos);
    bool WasBeingHovered() const { return m_isBeingHovered;}
    void ResetHoverState();

    bool MouseDetection(sf::Mouse::Button mouseButton, sf::Vector2i mousePos, const sf::Texture& transparentIconTexture, const sf::Texture& xIconTexture, const sf::Texture& queenIconTexture);

    void SetId(int id);
    int GetId() const;

    void SetColorId(int colorId);
    int GetColorId() const;

    void SetOriginalColor(const sf::Color& color);
    sf::Color GetOriginalColor() const;

    void SetCurrentColor(const sf::Color& color);
    sf::Color GetCurrentColor() const;

    void UpdateColor(sf::Color color);
    void ResetColor();

    void SetCoords(const sf::Vector2i& coords);
    sf::Vector2i GetCoords() const;

    void SetPosition(const sf::Vector2f& pos);
    sf::Vector2f GetPosition() const;

    void SetMark(const std::string& mark);
    std::string GetMark() const;

    sf::Vector2f GetSize() const;
    sf::FloatRect GetGlobalBounds() const;

    bool isMarkEmpty() const;
    bool isMarkX() const;
    bool isMarkQueen() const;

    void PlaceX(const sf::Texture& xIconTexture);
    void PlaceQueen(const sf::Texture& queenIconTexture);
    void ClearMark(const sf::Texture& transparentIconTexture);

private:
    sf::RectangleShape m_tile;
    sf::Sprite m_sprite;

    int m_id;
    int m_colorId;
    sf::Color m_originalColor;
    sf::Color m_currentColor;
    sf::Vector2i m_coords;
    sf::Vector2f m_pos;
    std::string m_mark;

    // -------
    bool m_isBeingHovered = false;
    // -------

    void SetTileIcon(const sf::Texture& texture);

    void DarkenColor();

    // -------

    friend std::ostream& operator<<(std::ostream& os, const CTile& tile);
};

#endif // TILE_HPP
