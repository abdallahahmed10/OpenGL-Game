#pragma once

#include <application.hpp>

#include <ecs/world.hpp>
#include <systems/forward-renderer.hpp>
#include <systems/free-camera-controller.hpp>
#include <systems/movement.hpp>
#include <asset-loader.hpp>

#include "../common/components/camera.hpp"
#include "../states/win-state.hpp"


namespace our {
    class EventHandler {
    public:
        Application* appForWinState;
        EventHandler(Application* app);

        // Method to check if the camera is within a certain x and y coordinate range
        void checkForWinCondition(const CameraComponent& camera);
    };

}
