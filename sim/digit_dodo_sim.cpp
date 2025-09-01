#include "digit_dodo_defines.h"

#include <SFML/Graphics.hpp>

void drawDigit(sf::RenderWindow& window, int digit, sf::Vector2f position, float size, bool showDot);
void drawNumber(sf::RenderWindow& window, const std::string& number, sf::Vector2f startPos, float size);



int main() {
    sf::RenderWindow window(sf::VideoMode(800, 300), "DigitDodo 7-Segment");

    std::string number = "43.123"; // You can change this dynamically

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        drawNumber(window, number, {50, 50}, 60);
        window.display();
    }

    return 0;
}


void drawDigit(sf::RenderWindow& window, int digit, sf::Vector2f position, float size, bool showDot)
{
    float thickness = size / 10;
    sf::Color onColor = sf::Color::Red;
    sf::Color offColor = sf::Color(50, 50, 50);

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
        segments[i].setFillColor(segmentMap[digit][i] ? onColor : offColor);
        window.draw(segments[i]);
    }

    // Draw dot if requested
    dot.setFillColor(showDot ? onColor : offColor);
    window.draw(dot);
}

void drawNumber(sf::RenderWindow& window, const std::string& number, sf::Vector2f startPos, float size)
{
    float spacing = size + size / 4;

    unsigned char pos = 0;

    for (size_t i = 0; i < number.size(); ++i) {
        char ch = number[i];
        bool showDot = (i + 1 < number.size() && number[i + 1] == '.');

        if (isdigit(ch)) {
            int digit = ch - '0';
            drawDigit(window, digit, {startPos.x + pos * spacing, startPos.y}, size, showDot);
            pos += 1;
        }
    }
}

