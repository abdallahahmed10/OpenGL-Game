#pragma once

#include "../ecs/component.hpp"

#include <glm/mat4x4.hpp>

namespace our {

    class CollideComponent: public Component
    {
        public:

            glm:: vec3 boundingBox;  //bounding box of the object

            
            // The ID of this component type is "Collision"
            static std::string getID() { return "collision"; }

            // Reads the bounding box from the given json object
            void deserialize(const nlohmann::json& data) override;
    };

}