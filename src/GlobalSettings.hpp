#ifndef GLOBAL_SETTINGS_HPP
#define GLOBAL_SETTINGS_HPP

#include "SFML/Graphics/Color.hpp"

namespace GlobalSettings
{
extern char WINDOW_TITLE[255];

inline const int WINDOW_WIDTH = 1280;
inline const int WINDOW_HEIGHT = 720;

inline const char LEVELS_PATH[255] = "./src/levels/";
inline const char TEXTURES_PATH[255] = "./src/textures/";

extern sf::Color BACKGROUND_COLOR;
extern float BACKGROUND_COLOR_INPUT[3];

inline const int TILE_SIZE = 64;
inline const int TILE_DARK_FACTOR = 32;
} // namespace GlobalSettings

#endif // GLOBAL_SETTINGS_HPP
