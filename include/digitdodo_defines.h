#pragma once

#include <unordered_map>

std::unordered_map<unsigned int, unsigned char> sevenSegmentDigitMap = {
    {0, 0b11111100},
    {1, 0b01100000},
    {2, 0b11011010},
    {3, 0b11110010},
    {4, 0b01100110},
    {5, 0b10110110},
    {6, 0b10111110},
    {7, 0b11100000},
    {8, 0b11111110},
    {9, 0b11110110}
};

std::unordered_map<char, unsigned char> sevenSegmentCharMap = {
    {' ', 0b00000000}
};