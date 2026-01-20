#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#include "vendor/imgui/imgui_internal.h"

const ImGuiWindowFlags dockspace_flags =
    ImGuiWindowFlags_NoDocking |
    ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoBringToFrontOnFocus |
    ImGuiWindowFlags_NoNavFocus;
