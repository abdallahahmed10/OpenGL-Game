#include "collide.hpp"
#include "../ecs/entity.hpp"
#include "../deserialize-utils.hpp"
#include <iostream>

namespace our
{
    // Reads the bounding box from the given json object
    void CollideComponent::deserialize(const nlohmann::json &data)
    {
        if (!data.is_object())
            return;
        boundingBox = glm::vec3(data.value("boundingBox", glm::vec3(0.0f, 0.0f, 0.0f)));
        std::cout<< "bounding box: " << boundingBox.x << " " << boundingBox.y << " " << boundingBox.z << std::endl;
    }
}