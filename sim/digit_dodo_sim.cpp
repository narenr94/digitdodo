#include "digit_dodo_sim.h"
#include "digit_dodo.h"

#include <unistd.h>
#include <vector>
#include <string>



int main() {
    int i = 0;
    std::vector<std::string> volt = { "10.01", "11.11", "12.21", "20.31", "204.1" };
    std::vector<std::string> curr = { "01.22", "11.22", "21.32", "01.42", "11.42" };

    digit_dodo& dodo = digit_dodo::getInstance();

    initWindow(800, 800, "DigitDodo 7-Segment");

    while (windowIsOpen()) {
        pollEvents();
        clearWindow();

        dodo.update_display_value("volt", volt[i]);
        dodo.update_display_value("curr", curr[i]);

        displayWindow();
        i = (i + 1) % 5;
        usleep(1000 * 1000);
    }

    destroyWindow();
    return 0;
}


