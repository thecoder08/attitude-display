#include <3d-engine/engine.h>
#include "serial.h"
#define __USE_MISC 1
#include <math.h>
#include <xgfx/window.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: display [serialPort]\n");
        return 1;
    }

    vec3** vertices;
    vec3** vertexNormals;
    int numMaterials;
    int* numVertices;
    vec3* colors;
    int wireframe = 0;
    vec3 lightPosition = {0, 0, -1};
    int lightIntensity = 1;
    if (loadObj("navball.obj", &numMaterials, &colors, &vertices, &vertexNormals, &numVertices)) {
        printf("Failed to load object file!\n");
        return 1;
    }
    initEngine(640, 480, "Navball", lightPosition, lightIntensity);

    int serial = open(argv[1], O_RDWR | O_NOCTTY | O_SYNC);
    if (serial < 0) {
        fprintf(stderr, "Error connecting to serial port: %s\n", strerror(errno));
        return 1;
    }
    set_interface_attribs(serial, B9600, 0);
    set_blocking(serial, 1);
    sleep(1);

    // tells transmitter to start sending data
    write(serial, "G", 1);
    while(1) {
        Event event;
        while(checkWindowEvent(&event)) {
            if (event.type == WINDOW_CLOSE) {
                return 0;
            }
            if (event.type == KEY_CHANGE) {
                if (event.keychange.key == 1 && event.keychange.state) {
                    wireframe = wireframe ? 0 : 1;
                }
            }
        }
        unsigned short pitch;
        read(serial, &pitch, 2);
        unsigned short yaw;
        read(serial, &yaw, 2);
        unsigned short roll;
        read(serial, &roll, 2);
        printf("%d %d %d\n", pitch, yaw, roll);
        mat3 rotationMatrixA;
        rotationMatrixXYZ((double)pitch/1023 * 6.283, (double)yaw/1023 * 6.283, (double)roll/1023 * 6.283, rotationMatrixA);
        for (int i = 0; i < numMaterials; i++) {
            renderObject(vertices[i], vertexNormals[i], numVertices[i], rotationMatrixA, colors[i], wireframe);
        }
        updateWindow3D();
    }
}
