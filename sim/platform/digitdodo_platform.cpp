#include "digitdodo_platform.h"
#include "digitdodo_sim.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <cmath>

std::vector<digitdodo_platform::HardwareGroup> hardwareGroups = {
    digitdodo_platform::HardwareGroup("volt", digitdodo_platform::SegmentDisplayType::SEG_7, 4),
    digitdodo_platform::HardwareGroup("curr", digitdodo_platform::SegmentDisplayType::SEG_7, 4),
    digitdodo_platform::HardwareGroup("ad", digitdodo_platform::SegmentDisplayType::SEG_7, 6),
    digitdodo_platform::HardwareGroup("14seg", digitdodo_platform::SegmentDisplayType::SEG_14, 6),

};

std::vector<digitdodo_platform::HardwareGroup>& digitdodo_platform::getGroups() {
    return hardwareGroups;
}

void drawNumber(const std::string& group, const std::vector<unsigned char>& number);

void digitdodo_platform::updateRawBuffer(const std::string& t_group_name, std::vector<unsigned char>& buffer)
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

// Requires: #include <SFML/Graphics.hpp>
// Assumes: m_window is an sf::RenderWindow* in scope
// maskBytes must be a vector<unsigned char> of size 2 (byte0 = bits 0..7, byte1 = bits 8..15)

void draw14SegmentFromBytes(const std::vector<unsigned char> &maskBytes, sf::Vector2f position, float size)
{
    if (maskBytes.size() < 2) return;

    auto isBitSet = [&](int bitIndex)->bool {
        if (bitIndex < 0 || bitIndex >= 16) return false;
        int byteIndex = bitIndex / 8;
        int bitInByte = bitIndex % 8;
        return (maskBytes[byteIndex] & (1u << bitInByte)) != 0;
    };

    const float thickness = size / 10.f;
    const sf::Color onColor = sf::Color::Red;
    const sf::Color offColor(10,10,10);

    const float w = size;
    const float h = 2.f * size;
    const float vlen = h * 0.38f;             // vertical segment length (approx)
    const float gWidth = w * 0.45f;           // center horizontals length
    const float gap = w * 0.06f;              // small gap between G1 and G2

    // horizontals A, G1, G2, D
    sf::RectangleShape horiz[4];
    horiz[0].setSize({w, thickness});         // A
    horiz[1].setSize({gWidth, thickness});    // G1 (left center)
    horiz[2].setSize({gWidth, thickness});    // G2 (right center)
    horiz[3].setSize({w, thickness});         // D

    // set origin to center for easy placement
    for (int i = 0; i < 4; ++i) horiz[i].setOrigin(horiz[i].getSize().x/2.f, horiz[i].getSize().y/2.f);

    // verticals: B, C, E, F, L, M
    sf::RectangleShape vert[6];
    for (int i = 0; i < 6; ++i) vert[i].setSize({thickness, vlen});
    vert[4].setSize({thickness * 0.8f, vlen * 0.9f}); // L
    vert[5].setSize({thickness * 0.8f, vlen * 0.9f}); // M
    for (int i = 0; i < 6; ++i) vert[i].setOrigin(vert[i].getSize().x/2.f, vert[i].getSize().y/2.f);

    // diagonals (quads)
    sf::ConvexShape diag[4];
    for (int i = 0; i < 4; ++i) diag[i].setPointCount(4);

    // decimal point
    sf::CircleShape dp(thickness * 0.6f);
    dp.setOrigin(dp.getRadius(), dp.getRadius());

    // reference coords (use centers)
    const sf::Vector2f topCenter(position.x + w*0.5f, position.y + thickness/2.f);
    const sf::Vector2f midCenter(position.x + w*0.5f, position.y + h*0.5f);
    const sf::Vector2f botCenter(position.x + w*0.5f, position.y + h - thickness/2.f);
    const float leftX = position.x + thickness/2.f;
    const float rightX = position.x + w - thickness/2.f;

    // place horizontals (centered by origin)
    horiz[0].setPosition(topCenter);                              // A
    horiz[1].setPosition(midCenter.x - (gap + gWidth)/2.f, midCenter.y); // G1 (left)
    horiz[2].setPosition(midCenter.x + (gap + gWidth)/2.f, midCenter.y); // G2 (right)
    horiz[3].setPosition(botCenter);                              // D

    // place verticals: use quarter positions for natural spacing
    vert[0].setPosition(rightX, position.y + h*0.25f);            // B upper-right
    vert[1].setPosition(rightX, position.y + h*0.75f);            // C lower-right
    vert[3].setPosition(leftX,  position.y + h*0.25f);            // F upper-left
    vert[2].setPosition(leftX,  position.y + h*0.75f);            // E lower-left

    // center verticals L (left center) and M (right center) aligned with G1/G2 edges
    float midLeftX  = horiz[1].getPosition().x + horiz[1].getSize().x/2.f;  // right edge of G1
    float midRightX = horiz[2].getPosition().x - horiz[2].getSize().x/2.f;  // left edge of G2
    float midVertY  = midCenter.y;
    // compute center X and half-gap between the two center horizontals
    const float centerX = position.x + w * 0.5f;
    const float halfGap = (gap + thickness) * 0.5f; // gap is the visual gap between G1 and G2

    // center verticals: L (upper center) and M (lower center) 
    const float centerGapY = midCenter.y; // y of the middle horizontal gap 
    const float upperCenterY = (topCenter.y + centerGapY) * 0.5f; // halfway between top and middle 
    const float lowerCenterY = (centerGapY + botCenter.y) * 0.5f; // halfway between middle and bottom

    // Y positions as before: upper and lower center
    vert[4].setPosition(centerX, upperCenterY);   // L (upper center)
    vert[5].setPosition(centerX, lowerCenterY);   // M (lower center)

    // diagonal endpoints (use inner offsets so diagonals butt to horizontals/verticals nicely)
    sf::Vector2f topLeft(position.x + thickness, position.y + thickness);
    sf::Vector2f topRight(position.x + w - thickness, position.y + thickness);
    sf::Vector2f midLeft(midLeftX - thickness*0.2f, midCenter.y);
    sf::Vector2f midRight(midRightX + thickness*0.2f, midCenter.y);
    sf::Vector2f botLeft(position.x + thickness, position.y + h - thickness);
    sf::Vector2f botRight(position.x + w - thickness, position.y + h - thickness);

    auto makeDiagonal = [&](sf::ConvexShape &c, sf::Vector2f p1, sf::Vector2f p2, float width) {
        sf::Vector2f dir = p2 - p1;
        float len = std::sqrt(dir.x*dir.x + dir.y*dir.y);
        if (len == 0.f) len = 1.f;
        sf::Vector2f n(-dir.y/len, dir.x/len);
        c.setPoint(0, p1 + n * (width/2.f));
        c.setPoint(1, p2 + n * (width/2.f));
        c.setPoint(2, p2 - n * (width/2.f));
        c.setPoint(3, p1 - n * (width/2.f));
    };

    makeDiagonal(diag[0], topLeft, midLeft, thickness);   // H
    makeDiagonal(diag[1], topRight, midRight, thickness); // I
    makeDiagonal(diag[2], midRight, botRight, thickness); // J
    makeDiagonal(diag[3], midLeft, botLeft, thickness);   // K

    dp.setPosition(position.x + w + thickness, position.y + h - dp.getRadius());

    // drawing helpers
    auto drawRect = [&](const sf::RectangleShape &s, bool on){
        sf::RectangleShape tmp = s; tmp.setFillColor(on ? onColor : offColor); m_window->draw(tmp);
    };
    auto drawConv = [&](const sf::ConvexShape &c, bool on){
        sf::ConvexShape tmp = c; tmp.setPointCount(c.getPointCount());
        for (std::size_t p = 0; p < c.getPointCount(); ++p) tmp.setPoint(p, c.getPoint(p));
        tmp.setFillColor(on ? onColor : offColor); m_window->draw(tmp);
    };

    // Map bits to segments
    drawRect(horiz[0], isBitSet(0));   // A
    drawRect(horiz[1], isBitSet(6));   // G1
    drawRect(horiz[2], isBitSet(7));   // G2
    drawRect(horiz[3], isBitSet(3));   // D

    drawRect(vert[0], isBitSet(1));    // B
    drawRect(vert[1], isBitSet(2));    // C
    drawRect(vert[3], isBitSet(5));    // F
    drawRect(vert[2], isBitSet(4));    // E
    drawRect(vert[4], isBitSet(12));   // L
    drawRect(vert[5], isBitSet(13));   // M

    drawConv(diag[0], isBitSet(8));    // H
    drawConv(diag[1], isBitSet(9));    // I
    drawConv(diag[2], isBitSet(10));   // J
    drawConv(diag[3], isBitSet(11));   // K

    dp.setFillColor(isBitSet(14) ? onColor : offColor);
    m_window->draw(dp);
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

void drawNumberString14Seg(const std::vector<unsigned char>& number, sf::Vector2f startPos, float size)
{
    float spacing = size + size / 4;

    for (size_t i = 0; i < number.size(); i+=2)
    {        
        draw14SegmentFromBytes(std::vector({number[i],number[i+1]}), {startPos.x + (i/2) * spacing, startPos.y}, size);
    }
}



void drawNumber(const std::string& group, const std::vector<unsigned char>& number) {
    if (group == "volt")
        drawNumberString(number, {50, 50}, 60);
    else if (group == "curr")
        drawNumberString(number, {50, 250}, 60);
    else if (group == "ad")
        drawNumberString(number, {50, 450}, 60);
    else if (group == "14seg")
        drawNumberString14Seg(number, {50, 250}, 60);

}
