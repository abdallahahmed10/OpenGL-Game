#include "camera.hpp"
#include "../ecs/entity.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace our
{
    // Reads camera parameters from the given json object
    void CameraComponent::deserialize(const nlohmann::json &data)
    {
        if (!data.is_object())
            return;
        std::string cameraTypeStr = data.value("cameraType", "perspective");
        if (cameraTypeStr == "orthographic")
        {
            cameraType = CameraType::ORTHOGRAPHIC;
        }
        else
        {
            cameraType = CameraType::PERSPECTIVE;
        }
        near = data.value("near", 0.01f);
        far = data.value("far", 100.0f);
        fovY = data.value("fovY", 90.0f) * (glm::pi<float>() / 180);
        orthoHeight = data.value("orthoHeight", 1.0f);
    }

    // Creates and returns the camera view matrix
    glm::mat4 CameraComponent::getViewMatrix() const
    {
        auto owner = getOwner();
        auto M = owner->getLocalToWorldMatrix();
        // TODO: (Req 8) Complete this function
        // HINT:
        //  In the camera space:
        //  - eye is the origin (0,0,0)
        //  - center is any point on the line of sight. So center can be any point (0,0,z) where z < 0. For simplicity, we let center be (0,0,-1)
        //  - up is the direction (0,1,0)
        //  but to use glm::lookAt, we need eye, center and up in the world state.
        //  Since M (see above) transforms from the camera to thw world space, you can use M to compute:
        //  - the eye position which is the point (0,0,0) but after being transformed by M
        //  - the center position which is the point (0,0,-1) but after being transformed by M
        //  - the up direction which is the vector (0,1,0) but after being transformed by M
        //  then you can use glm::lookAt

        // Calculate eye, center, and up vectors in world space using the matrix M
        glm::vec3 eye = glm::vec3(M[3]); // basically the translation column in camera to world space matrix is the position of camera in world space
        glm::vec3 center = eye + glm::vec3(M * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
        glm::vec3 up = glm::vec3(M * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));

        // Use glm::lookAt to create the view matrix
        return glm::lookAt(eye, center, up);

        // return glm::mat4(1.0f);
    }

    // Creates and returns the camera projection matrix (whic we need to transform view space into NDC)
    // "viewportSize" is used to compute the aspect ratio
    glm::mat4 CameraComponent::getProjectionMatrix(glm::ivec2 viewportSize) const
    {
        // TODO: (Req 8) Wrtie this function
        //  NOTE: The function glm::ortho can be used to create the orthographic projection matrix
        //  It takes left, right, bottom, top. Bottom is -orthoHeight/2 and Top is orthoHeight/2.
        //  Left and Right are the same but after being multiplied by the aspect ratio
        //  For the perspective camera, you can use glm::perspective

        // float aspectRatio = static_cast<float>(viewportSize.x) / static_cast<float>(viewportSize.y);

        if (cameraType == CameraType::ORTHOGRAPHIC)
        {
            // Compute aspect ratio

            // Use glm::ortho to create the orthographic projection matrix
            // orthoheight is height of camera if it is orthographic, it is property of this component
            // so if orthoheight = 10, then top = 5 and bottom = -5
            // now if aspect ratio = 5, then orthowidth = 50. Then left = -orthowidth/2, right = othrowidth/2
            // near/far also properties of this component
            float aspectRatio = static_cast<float>(viewportSize.x) / static_cast<float>(viewportSize.y);

            return glm::ortho(-orthoHeight * 0.5f * aspectRatio, orthoHeight * 0.5f * aspectRatio, -orthoHeight * 0.5f, orthoHeight * 0.5f, near, far);
        }
        else
        {
            // Use glm::perspective to create the perspective projection matrix
            // return glm::perspective(fovY, aspectRatio, near, far);
            return glm::perspective(fovY, static_cast<float>(viewportSize.x) / static_cast<float>(viewportSize.y), near, far);
        }

        // return glm::mat4(1.0f);
    }
}