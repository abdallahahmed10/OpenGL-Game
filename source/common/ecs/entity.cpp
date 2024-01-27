#include "entity.hpp"
#include "../deserialize-utils.hpp"
#include "../components/component-deserializer.hpp"

#include <glm/gtx/euler_angles.hpp>

namespace our
{

    // This function returns the transformation matrix from the entity's local space to the world space
    // Remember that you can get the transformation matrix from this entity to its parent from "localTransform"
    // To get the local to world matrix, you need to combine this entities matrix with its parent's matrix and
    // its parent's parent's matrix and so on till you reach the root.
    glm::mat4 Entity::getLocalToWorldMatrix() const
    {

        // TODO: (Req 8) Write this function

        if (!parent) // base case: the entity has no parent
        {
            return localTransform.toMat4();
        }
        else // recursive case: the entity has a parent so we will combine the local to world matrix of the parent with the local to world matrix of this entity
            return parent->getLocalToWorldMatrix() * localTransform.toMat4();

        // glm::mat4 localToWorldMatrix(1.0f); // Start with an identity matrix

        // // Traverse the hierarchy from this entity to the root
        // const Entity *currentEntity = this;
        // // this loop will terminate when we're done applying the matrix transformation of an entity whose parent is the world space
        // // How? well if an entity's parent is null/entity is world, our loop will continue running one more time to transform us to space of that entity
        // while (currentEntity != nullptr)
        // {
        //     // local entity: entity currently represented by "currentEntity" variabke
        //     // owning entity: entity that called this function
        //     //  Combine the local transformation matrix of the current entity with the accumulated matrix

        //     // the first operand is the matrix transforming local entity space to its parents space (since we store it as Transform in entity class we convert it to matrix first to allow for matrix multiplication)
        //     // the 2nd operand is the matrix transforming us from us from the owning entity space to local entity space
        //     localToWorldMatrix = (currentEntity->localTransform).toMat4() * localToWorldMatrix;

        //     // Move to the parent entity
        //     currentEntity = currentEntity->parent;
        // }

        // return localToWorldMatrix;
    }

    // Deserializes the entity data and components from a json object
    void Entity::deserialize(const nlohmann::json &data)
    {
        if (!data.is_object())
            return;
        name = data.value("name", name);
        localTransform.deserialize(data);
        if (data.contains("components"))
        {
            if (const auto &components = data["components"]; components.is_array())
            {
                for (auto &component : components)
                {
                    deserializeComponent(component, this);
                }
            }
        }
    }

}