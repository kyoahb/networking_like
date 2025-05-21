#include "MainMenu.h"
#include "Game/Game.h"

MainMenu::MainMenu(Game& _game) : GameState(_game, "MainMenu") {

}

MainMenu::~MainMenu() {
	// Destructor implementation
}

void MainMenu::on_draw() {
	// Draw the main menu
    ImGui::SetNextWindowPos({ 0, 0 });
    ImGui::SetNextWindowSize({ (float)GetScreenWidth(), (float)GetScreenHeight() });

    ImGui::Begin("FullscreenWindow", NULL,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoBringToFrontOnFocus
    );

    ImGui::Text("Game");

    std::string address = "127.0.0.1";
    int port = 7777;

    if (ImGui::Button("Host", ImVec2(200, 100))) {
        game.create_server(address, port);
		game.create_client(address, port);

        Log::asserts(game.client->is_connected(), "Failed to connect to server from MainMenu");
        if (!game.stateManager.setState("Lobby")) {
            Log::asserts(false, "Failed to set state to Lobby from MainMenu");
        }
    }

    if (ImGui::Button("Join", ImVec2(200, 100))) {
        game.create_client(address, port);

		Log::asserts(game.client->is_connected(), "Failed to connect to server from MainMenu");
        if (!game.stateManager.setState("Lobby")) {
            Log::asserts(false, "Failed to set state to Lobby from MainMenu");
        }
    }

    if (ImGui::Button("Exit", ImVec2(200, 100))) {
        onExitButtonClick();
    }  

    ImGui::End();
}