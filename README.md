Forked from https://github.com/eliasdaler/cmake-fetchcontent-tutorial-code.

# Introduction

'Queens' project developed with `SFML`, `imgui` and `imgui-sfml`.

Based on [Queens, by LinkedIn](https://www.linkedin.com/showcase/queens-game/)

[queens-game-screenshot]: images/overview_game.png
[queens-rules-screenshot]: images/overview_rules.png

[![Queens rules screenshot][queens-rules-screenshot]]()
[![Queens game screenshot][queens-game-screenshot]]()

# Features

- Set program title.
- FPS shown in UI.
- Set background color of program.
- Popup with game rules.
- Load levels.
- Play the 'Queens' game and check if you've solved the level.
- Implemented rules to check if game is completed successfully.

# TODO

- Hints system.
- Algorithm to solve level.
- Highlight offending queens when they are not right in a given level.
- Clearly state a level has been completed successfully.

# Building and running

```sh
cd queens
cmake -S . -B build
cmake --build build
./build/src/queens
```

Alternatively, you can build and run 'Queens' using Visual Studio Code with the launch configuration provided.
