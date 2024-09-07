#pragma once
#include <ui/button.hpp>
#include <vector>

struct List
{
    std::vector<BuildedButton> buttons = std::vector<BuildedButton>();
    int x, y = 0;
    bool visible = false;
    bool prevVisible = false;
};

struct ListInfo
{
    std::vector<ButtonInfo> buttons;
    int x, y;
};

struct BuildedList
{
    List list;
    ListInfo listInfo;
};