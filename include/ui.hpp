#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#include "vendor/imgui/imgui_internal.h"
#include "vendor/butia/butia.hpp"

const ImGuiWindowFlags dockspace_flags =
    ImGuiWindowFlags_NoDocking |
    ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoBringToFrontOnFocus |
    ImGuiWindowFlags_NoNavFocus;

void ui(Butia* butia, GLFWwindow *window);
void input(Butia* butia, GLFWwindow *window);
