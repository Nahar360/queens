#include "UiSettings.hpp"

#include <climits>

namespace UiSettings
{
int LEVEL_COLS = -1;
int LEVEL_ROWS = -1;

int LEVEL_CURRENT_INDEX = 0;

bool SHOW_DEBUG_OPTIONS = false;

bool LEVEL_COMPLETED = false;
int LEVEL_COMPLETED_TIME = INT_MAX;
bool POPUP_HAS_BEEN_CLOSED = false;
} // namespace UiSettings
