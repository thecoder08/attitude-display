#include <xgfx/window.h>
#include <xgfx/drawing.h>
#include <math.h>
#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "serial.h"

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

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: display [serialPort]\n");
        return 1;
    }
    int serial = open(argv[1], O_RDWR | O_NOCTTY | O_SYNC);
    if (serial < 0) {
        fprintf(stderr, "Error connecting to serial port: %s\n", strerror(errno));
        return 1;
    }
    set_interface_attribs(serial, B9600, 0);
    set_blocking(serial, 1);
    sleep(1);

    initWindow(640, 960, "Attitude & Heading");
    // tells transmitter to start sending data
    write(serial, "G", 1);
    while(1) {
        Event event;
        while(checkWindowEvent(&event)) {
            if (event.type == WINDOW_CLOSE) {
                return 0;
            }
        }
        unsigned short pitch;
        read(serial, &pitch, 2);
        unsigned short yaw;
        read(serial, &yaw, 2);
        unsigned short roll;
        read(serial, &roll, 2);
        printf("%d %d %d\n", pitch, yaw, roll);
        drawScreen(tan((double)roll/1023 * 6.28), sin((double)pitch/1023 * 6.28)*240);
        rectangle(213, 238, 213, 4, 0xffffffff);
        rectangle(318, 133, 4, 213, 0xffffffff);

        rectangle(0, 480, 640, 480, 0xff000000);
        for (unsigned short angle = 0; angle < 512; angle += 128) {
            double x = sin((double)(yaw + angle)/1023 * 6.28);
            double y = cos((double)(yaw + angle)/1023 * 6.28);
            line(150*-x + 320, 150*-y + 720, 150*x + 320, 150*y + 720, 0xffffffff);
        }
        circle(320, 720, 133, 0xffffffff);
        circle(320, 720, 123, 0xff000000);
        rectangle(310, 710, 20, 20, 0xff00ff00);
        updateWindow();
    }
}