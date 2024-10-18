#include <SFML/Graphics.hpp>

namespace GlobalSettings
{
char WINDOW_TITLE[255] = "Queens";

int WINDOW_WIDTH = 1280;
int WINDOW_HEIGHT = 720;

char TEXTURES_PATH[255] = "./src/textures/";
char WORLDS_PATH[255] = "./src/worlds/";

sf::Color BACKGROUND_COLOR = {105, 105, 105};
float BACKGROUND_COLOR_INPUT[3] = {0.41f, 0.41f, 0.41f};

int TILE_SIZE = 64;
int TILE_DARK_FACTOR = 32;
} // namespace GlobalSettings
