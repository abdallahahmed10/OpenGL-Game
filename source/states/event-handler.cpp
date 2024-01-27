#include "event-handler.hpp"
#include <iostream>

namespace our {

    EventHandler::EventHandler(Application* app) {
        // Constructor implementation (if needed)
        appForWinState = app;
    }

    void EventHandler::checkForWinCondition(const CameraComponent& camera) {
        // Define the x and y coordinate range for winning condition
        const float minX = 21;
        const float maxX = 28.5;
        const float minZ = 41.5;
        const float maxZ = 48;

        // Get the camera position from its world transformation matrix
        glm::vec3 cameraPos = camera.getOwner()->getLocalToWorldMatrix()[3];

        // Check if the camera is within the defined range
        if (cameraPos.x >= minX && cameraPos.x <= maxX &&
            cameraPos.z >= minZ && cameraPos.z <= maxZ) {
            // If within range, trigger the win state
            //WinState::showWinningScreen();
            std::cout << "You win!" << std::endl;
            // WinState winStateInstance;
            //winStateInstance.onInitialize();
            appForWinState->changeState("win");
        }
    }

}
