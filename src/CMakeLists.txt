add_executable(queens
  Game.cpp
  Game.hpp
  GlobalSettings.cpp
  GlobalSettings.hpp
  UiManager.cpp
  UiManager.hpp
  UiSettings.cpp
  UiSettings.hpp
  Tile.cpp
  Tile.hpp
  World.hpp
  World.cpp
  main.cpp
)

target_link_libraries(queens PRIVATE ImGui-SFML::ImGui-SFML)

# Copy DLLs needed for runtime on Windows
if(WIN32)
  if (BUILD_SHARED_LIBS)
    add_custom_command(TARGET queens POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
          $<TARGET_FILE:ImGui-SFML::ImGui-SFML>
          $<TARGET_FILE:sfml-graphics>
          $<TARGET_FILE:sfml-window>
          $<TARGET_FILE:sfml-system>
          $<TARGET_FILE_DIR:queens>)
  endif()
endif()
