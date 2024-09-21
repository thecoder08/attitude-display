#include <xgfx/window.h>
#include <xgfx/drawing.h>
#include <math.h>

void drawScreen(double m, double b) {
    for (int i = 0; i < 480; i++) {
        for (int j = 0; j < 640; j++) {
            int x = j - 320;
            int y = -(i - 240);
            if (y > m*x+b + 5) {
                plot(j, i, 0xff55aaff);
            }
            else if (y < m*x+b - 5) {
                plot(j, i, 0xffff5500);
            }
            else {
                plot(j, i, 0xffffffff);
            }
        }
    }
}

int main() {
    initWindow(640, 960, "Attitude & Heading");
    unsigned char roll = 0;
    unsigned char pitch = 0;
    unsigned char yaw = 0;
    while(1) {
        Event event;
        while(checkWindowEvent(&event)) {
            if (event.type == WINDOW_CLOSE) {
                return 0;
            }
        }
        roll++;
        pitch++;
        yaw++;
        drawScreen(tan((double)roll/255 * 6.28), sin((double)pitch/255 * 6.28)*240);
        rectangle(213, 238, 213, 4, 0xffffffff);
        rectangle(318, 133, 4, 213, 0xffffffff);

        rectangle(0, 480, 640, 480, 0xff000000);
        for (char angle = 0; angle < 128; angle += 32) {
            double x = sin((double)(yaw + angle)/255 * 6.28);
            double y = cos((double)(yaw + angle)/255 * 6.28);
            line(150*-x + 320, 150*-y + 720, 150*x + 320, 150*y + 720, 0xffffffff);
        }
        circle(320, 720, 133, 0xffffffff);
        circle(320, 720, 123, 0xff000000);
        rectangle(310, 710, 20, 20, 0xff00ff00);
        updateWindow();
    }
}