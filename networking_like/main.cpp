#include <iostream>
#include "Utils/Imports/common.h"
#include "Utils/Input/Input.h"

#include "Networking/Packets/Packet.h"

int main()
{
    Log::info("Program started!");

    // Initialise enet
    int enet_status = enet_initialize();
    Log::asserts(enet_status == 0, "An error occurred while initializing ENet.");
    atexit(enet_deinitialize); // Ensure ENet is deinitialized on exit

    // Initialise window
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    raylib::Window window(screenWidth, screenHeight, "raylib [core] example - 3d camera first person");


    while (!window.ShouldClose()) {
        Input::update();

        // Draw
		BeginDrawing();
        window.ClearBackground(WHITE);


		raylib::DrawText("Hello World", 190, 200, 20, raylib::RED);
        EndDrawing();


    }

}
