#include "digit_dodo_platform.h"
#include "digit_dodo_sim.h"
#include "digit_dodo_defines.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

std::vector<digit_dodo_platform::HardwareGroup> hardwareGroups = {
    digit_dodo_platform::HardwareGroup("volt", {digit_dodo_platform::SegmentDisplayType::SEG_7, digit_dodo_platform::SegmentDisplayType::SEG_7, digit_dodo_platform::SegmentDisplayType::SEG_7, digit_dodo_platform::SegmentDisplayType::SEG_7}),
    digit_dodo_platform::HardwareGroup("curr", {digit_dodo_platform::SegmentDisplayType::SEG_7, digit_dodo_platform::SegmentDisplayType::SEG_7, digit_dodo_platform::SegmentDisplayType::SEG_7, digit_dodo_platform::SegmentDisplayType::SEG_7})
};

std::vector<digit_dodo_platform::HardwareGroup>& digit_dodo_platform::getGroups() {
    return hardwareGroups;
}

void drawNumber(const std::string& group, const std::vector<unsigned char>& number);

void digit_dodo_platform::updateRawBuffer(const std::string& t_group_name, std::vector<unsigned char>& buffer)
{
    drawNumber(t_group_name, buffer); // Draw new display
}

static sf::RenderWindow* m_window = nullptr;

void initWindow(int width, int height, const std::string& title) {
    m_window = new sf::RenderWindow(sf::VideoMode(width, height), title);
}

bool windowIsOpen() {
    return m_window && m_window->isOpen();
}

void pollEvents() {
    sf::Event event;
    while (m_window->pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            m_window->close();
    }
}

void clearWindow() {
    m_window->clear();
}

void displayWindow() {
    m_window->display();
}

void closeWindow() {
    if (m_window) m_window->close();
}

void destroyWindow() {
    delete m_window;
    m_window = nullptr;
}

void drawDigit(unsigned char digit, sf::Vector2f position, float size)
{
    float thickness = size / 10;
    sf::Color onColor = sf::Color::Red;
    sf::Color offColor = sf::Color(10, 10, 10);

    std::vector<sf::RectangleShape> segments(7);
    sf::CircleShape dot(thickness / 2);

    // Segment definitions (A–G)
    segments[0] = sf::RectangleShape({size, thickness}); // A
    segments[1] = sf::RectangleShape({thickness, size}); // B
    segments[2] = sf::RectangleShape({thickness, size}); // C
    segments[3] = sf::RectangleShape({size, thickness}); // D
    segments[4] = sf::RectangleShape({thickness, size}); // E
    segments[5] = sf::RectangleShape({thickness, size}); // F
    segments[6] = sf::RectangleShape({size, thickness}); // G

    // Dot as a small circle
    dot.setPosition(position.x + size + thickness, position.y + 2 * size + thickness);

    // Position segments
    segments[0].setPosition(position.x, position.y);                       // A
    segments[1].setPosition(position.x + size - thickness, position.y);   // B
    segments[2].setPosition(position.x + size - thickness, position.y + size); // C
    segments[3].setPosition(position.x, position.y + 2 * size);           // D
    segments[4].setPosition(position.x, position.y + size);               // E
    segments[5].setPosition(position.x, position.y);                      // F
    segments[6].setPosition(position.x, position.y + size);              // G

    // Draw segments
    for (int i = 0; i < 7; ++i) {
        segments[i].setFillColor((digit & (0x01 << (7 - i))) ? onColor : offColor);
        m_window->draw(segments[i]);
    }

    // Draw dot if requested
    dot.setFillColor((digit & 0x01) ? onColor : offColor);
    m_window->draw(dot);
}

void drawNumberString(const std::vector<unsigned char>& number, sf::Vector2f startPos, float size)
{
    float spacing = size + size / 4;

    for (size_t i = 0; i < number.size(); ++i)
    {        
        drawDigit(number[i], {startPos.x + i * spacing, startPos.y}, size);
    }
}



void drawNumber(const std::string& group, const std::vector<unsigned char>& number) {
    if (group == "volt")
        drawNumberString(number, {50, 50}, 60);
    else if (group == "curr")
        drawNumberString(number, {50, 250}, 60);
}
