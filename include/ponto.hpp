#pragma once

#include "assets.hpp"

class Default_game_objects {
    public:
    unsigned int grid, seta;
};

extern Default_game_objects default_game_objects;
extern unsigned int selected_object;
extern bool is_terminal_open;
extern Assets assets;
