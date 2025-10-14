#include "digitdodo_sim.h"
#include "digitdodo.h"

#include <unistd.h>
#include <vector>
#include <string>

#define TICK_TIME_MS 100

std::vector<std::string> volt = { "12.34567", "11.11", "12.21", "20.31", "204.1" };
std::vector<std::string> curr = { "01.22", "11.22", "21.32", "01.42", "11.42" };

std::string scroll_val =
    "012.3-456=7 89-_:,=+?!*%#@;ABCDEFGHIJKLMNOPQR.STUVWXYZabcdefghijklmnopqrstuvwxyz()<>[]{}$&^`~|";

int i = 0;

void update_mode(void* ctx)
{
    static bool toggle = false;
    digitdodo& dd = digitdodo::getInstance();
    std::string group_name = std::string((const char*) ctx);
    if(toggle)
    {
        toggle = false;
        dd.set_display_mode(group_name, SevenSegmentDisplayMode::Scroll, 500);
    }
    else
    {
        toggle = true;
        dd.set_display_mode(group_name, SevenSegmentDisplayMode::Blink, 1000);
    }
}

void update_trampolines(void* ctx)
{
    digitdodo& dd = digitdodo::getInstance();
    std::string group_name = std::string((const char*) ctx);
    dd.update_display(group_name);
    i++;
    if(i >= 5) i = 0;
    dd.update_display_value(group_name, volt[i]);
}

void toggle_scroll_direction(void* ctx)
{
    static bool rightToLeft = true;
    digitdodo& dd = digitdodo::getInstance();
    std::string group_name = std::string((const char*) ctx);
    if(rightToLeft)
    {
        rightToLeft = false;
        dd.set_scroll_direction(group_name, digitdodo::ScrollDirection::RightToLeft);
    }
    else
    {
        rightToLeft = true;
        dd.set_scroll_direction(group_name, digitdodo::ScrollDirection::LeftToRight);
    }
    
}

int main() {
    
    std::string seg14name = "14seg";
    std::string voltName = "volt";

    std::string adname = "ad";

    digitdodo& dodo = digitdodo::getInstance();

    TickToucan& tt = TickToucan::instance();
    tt.init(TICK_TIME_MS); 

    initWindow(800, 800, "DigitDodo 7-Segment");

    // dodo.update_display_value("curr", curr[2]);

    // dodo.set_display_mode("curr", SevenSegmentDisplayMode::Blink, 500);

    dodo.update_display_value(adname, scroll_val);

    dodo.set_scroll_direction(adname, digitdodo::ScrollDirection::RightToLeft); 

    dodo.set_display_mode(adname, SevenSegmentDisplayMode::Scroll, 500);

    dodo.update_display_value(seg14name, scroll_val);

    dodo.set_scroll_direction(seg14name, digitdodo::ScrollDirection::RightToLeft); 

    // dodo.set_display_mode(seg14name, SevenSegmentDisplayMode::Blink, 500);

    tt.scheduleEvery(15000, &update_mode, (void*)seg14name.c_str());

    tt.scheduleEvery(1500, &update_trampolines, (void*)voltName.c_str(), 5000);

    while (windowIsOpen()) {

        tt.dispatch();
        pollEvents();
        // clearWindow();        

        displayWindow();

        usleep(1000 * TICK_TIME_MS);
    }

    destroyWindow();
    return 0;
}


