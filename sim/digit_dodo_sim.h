#pragma once

#include <string>

void initWindow(int width, int height, const std::string& title);

bool windowIsOpen();

void pollEvents();

void clearWindow();

void displayWindow();

void closeWindow();

void destroyWindow();