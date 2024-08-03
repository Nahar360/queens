#ifndef WORLD_HPP
#define WORLD_HPP

#include <optional>
#include <string>
#include <vector>

#include "Tile.hpp"

class CWorld
{
public:
    CWorld() = default;
    ~CWorld() = default;

    void Init();

    void Clear();
    void Update(sf::RenderWindow& window);
    void MouseDetection(sf::Mouse::Button mouseButton, sf::Vector2i mousePos);

    void PrintRepresentation();

    void Load(const std::string& worldFileName);

private:
    // Tiles
    std::vector<std::vector<CTile>> m_tiles;

    void InitTilesFromRepr(const std::vector<std::vector<int>>& repr);
};

#endif // WORLD_HPP
