#include "digit_dodo_defines.hpp"
#include "digit_dodo_sim.hpp"
#include "digit_dodo.hpp"

#include <cassert>
#include <unistd.h>
#include <iostream>

void drawDigit(sf::RenderWindow& window, int digit, sf::Vector2f position, float size, bool showDot);
void drawNumberString(sf::RenderWindow& window, const std::string& number, sf::Vector2f startPos, float size);
void drawNumberInt(sf::RenderWindow& window, const int& number, sf::Vector2f startPos, float size);

SimI7SegmentPlatformImp::SimI7SegmentPlatformImp(): m_window(sf::VideoMode(800, 800), "DigitDodo 7-Segment")
{
    // Hardcoded for now; could be loaded from file
    m_displayCount = 4;
    std::vector<HardwareGroup> hwSpec;

    std::vector<SegmentDisplayType> group1_seg_types;
    std::string group1_name = "volt";
    group1_seg_types.push_back(SegmentDisplayType::SEG_7);
    group1_seg_types.push_back(SegmentDisplayType::SEG_7);
    group1_seg_types.push_back(SegmentDisplayType::SEG_7);
    group1_seg_types.push_back(SegmentDisplayType::SEG_7);
    hwSpec.emplace_back(group1_name, group1_seg_types);

    std::vector<SegmentDisplayType> group2_seg_types;
    std::string group2_name = "curr";
    group2_seg_types.push_back(SegmentDisplayType::SEG_7);
    group2_seg_types.push_back(SegmentDisplayType::SEG_7);
    group2_seg_types.push_back(SegmentDisplayType::SEG_7);
    group2_seg_types.push_back(SegmentDisplayType::SEG_7);
    hwSpec.emplace_back(group2_name, group2_seg_types);

    m_groups = hwSpec;

}

int SimI7SegmentPlatformImp::getDisplayCount() const
{
    return m_displayCount;
}

std::vector<HardwareGroup> SimI7SegmentPlatformImp::getGroups() const
{
    return m_groups;
}

sf::RenderWindow& SimI7SegmentPlatformImp::getWindow()
{
    return m_window;
}

void SimI7SegmentPlatformImp::updateRawBuffer(const std::string& t_group_name, const std::string& buffer)
{
    if (t_group_name == "volt")
    {
        drawNumberString(m_window, buffer, {50, 50}, 60);
    }
    else if (t_group_name == "curr")
    {
        drawNumberString(m_window, buffer, {50, 250}, 60);
    }
}

SimI7SegmentPlatformImp::~SimI7SegmentPlatformImp()
{

}

int main()
{
    int i = 0;

    std::vector<std::string> volt;

    volt.push_back("10.01");
    volt.push_back("11.11");
    volt.push_back("12.21");
    volt.push_back("20.31");
    volt.push_back("204.1");

    std::vector<std::string> curr;
    
    curr.push_back("01.22");
    curr.push_back("11.22");
    curr.push_back("21.32");
    curr.push_back("01.42");
    curr.push_back("11.42");

    I7SegmentPlatform* sim = new SimI7SegmentPlatformImp();

    digit_dodo* mid = new digit_dodo(sim);

    std::string volt_str = "volt";
    std::string curr_str = "curr";

    sf::RenderWindow* m_window = nullptr;

    if (auto* simImp = dynamic_cast<SimI7SegmentPlatformImp*>(sim))
    {
        m_window = &simImp->getWindow();
    }
    else
    {
        std::cout << " cant dynamic cast\n";
        return -1;
    }


    while (m_window->isOpen()) {
        sf::Event event;
        while (m_window->pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                m_window->close();
        }

        m_window->clear();

        mid->update_display_value(volt_str, volt[i]);
        mid->update_display_value(curr_str, curr[i]);
        
        m_window->display();
        i++;
        if(i >= 5)
        {
            i = 0;
        }
        usleep(1000 * 1000);
    }

    delete sim;

    return 0;
}

void drawDigit(sf::RenderWindow& window, int digit, sf::Vector2f position, float size, bool showDot)
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
        segments[i].setFillColor(segmentDigitMap[digit][i] ? onColor : offColor);
        window.draw(segments[i]);
    }

    // Draw dot if requested
    dot.setFillColor(showDot ? onColor : offColor);
    window.draw(dot);
}

void drawNumberString(sf::RenderWindow& window, const std::string& number, sf::Vector2f startPos, float size)
{
    float spacing = size + size / 4;

    unsigned char num_count = 0;

    constexpr int NUM_DIGITS = 4;

    for (size_t i = 0; num_count < NUM_DIGITS; ++i) {
        char ch = number[i];
        bool showDot = (i < (NUM_DIGITS) && number[i + 1] == '.');

        if (isdigit(ch)) {
            int digit = ch - '0';
            drawDigit(window, digit, {startPos.x + num_count * spacing, startPos.y}, size, showDot);
            num_count += 1;
        }
    }
}

