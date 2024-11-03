#ifndef TILE_HPP
#define TILE_HPP

#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Window/Mouse.hpp"

enum class Mark
{
    EMPTY,
    X,
    QUEEN
};

class Tile
{
public:
    Tile(int id, int colorId, sf::Color color, sf::Vector2i coords, sf::Vector2f pos);

    ~Tile() = default;

    void Init();

    void Draw(sf::RenderWindow& window);

    bool MouseHover(sf::Vector2i mousePos);
    bool WasBeingHovered() const { return m_isBeingHovered;}
    void ResetHoverState();

    bool MouseDetection(sf::Mouse::Button mouseButton, sf::Vector2i mousePos);

    void SetId(int id) { m_id = id; }
    int GetId() const { return m_id; }

    void SetColorId(int colorId) { m_colorId = colorId; }
    int GetColorId() const { return m_colorId; }

    void SetOriginalColor(const sf::Color& color) { m_originalColor = color; }
    sf::Color GetOriginalColor() const { return m_originalColor; }

    void SetCurrentColor(const sf::Color& color) { m_currentColor = color; }
    sf::Color GetCurrentColor() const { return m_currentColor; }

    void SetCoords(const sf::Vector2i& coords) { m_coords = coords; }
    const sf::Vector2i& GetCoords() const { return m_coords; }

    void SetPosition(const sf::Vector2f& pos) { m_pos = pos; }
    const sf::Vector2f& GetPosition() const { return m_pos; }

    void SetMark(Mark mark) { m_mark = mark; }
    Mark GetMark() const { return m_mark; }

    sf::Vector2f GetSize() const;
    sf::FloatRect GetGlobalBounds() const;

    void UpdateColor(sf::Color color);
    void ResetColor();

    bool isMarkEmpty() const;
    bool isMarkX() const;
    bool isMarkQueen() const;

    void PlaceX();
    void PlaceQueen();
    void ClearMark();

private:
    sf::RectangleShape m_tile;
    sf::Sprite m_sprite;

    int m_id;
    int m_colorId;
    sf::Color m_originalColor;
    sf::Color m_currentColor;
    sf::Vector2i m_coords;
    sf::Vector2f m_pos;
    Mark m_mark;

    // -------
    bool m_isBeingHovered = false;
    // -------

    void SetSprite(const sf::Texture& texture, const float offset = 0.0f, const sf::Vector2f& scale = {1.0f, 1.0f});

    void DarkenColor();

    // -------

    friend std::ostream& operator<<(std::ostream& os, const Tile& tile);
};

#endif // TILE_HPP
