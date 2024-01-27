#include "forward-renderer.hpp"
#include "../mesh/mesh-utils.hpp"
#include "../texture/texture-utils.hpp"
#include <iostream>
#include <string>
using namespace std;

namespace our
{

    void ForwardRenderer::initialize(glm::ivec2 windowSize, const nlohmann::json &config)
    {
        // First, we store the window size for later use
        this->windowSize = windowSize;

        // Then we check if there is a sky texture in the configuration
        if (config.contains("sky"))
        {
            // First, we create a sphere which will be used to draw the sky
            this->skySphere = mesh_utils::sphere(glm::ivec2(16, 16));

            // We can draw the sky using the same shader used to draw textured objects
            ShaderProgram *skyShader = new ShaderProgram();
            skyShader->attach("assets/shaders/textured.vert", GL_VERTEX_SHADER);
            skyShader->attach("assets/shaders/textured.frag", GL_FRAGMENT_SHADER);
            skyShader->link(); // linking vertex shader to fragment shader
            // During rendering, the textured object's vertices are transformed in the vertex shader, and the fragment shader is responsible for fetching the texture color based on the interpolated texture coordinates.
            //  texture coordinates: el (u,v) that tells me where vertex "x" is on the texture

            // TODO: (Req 10) Pick the correct pipeline state to draw the sky
            //  Hints: the sky will be draw after the opaque objects so we would need depth testing but which depth funtion should we pick?
            //  We will draw the sphere from the inside, so what options should we pick for the face culling.
            PipelineState skyPipelineState{};
            skyPipelineState.depthTesting.enabled = true;
            skyPipelineState.depthTesting.function = GL_LEQUAL; // To ensure that the sky is drawn behind all other objects.
            // sky has maximum depth value
            // By using GL_LEQUAL, the sky geometry passes the depth test if its fragments have a depth value that is less than or equal to the current contents of the depth buffer (which is usually set to the far plane’s maximum z value of 1.0 after a glClear(GL_DEPTH_BUFFER_BIT) call)

            skyPipelineState.faceCulling.enabled = true;
            skyPipelineState.faceCulling.culledFace = GL_FRONT; // Since we're inside the sphere, cull front faces.
            // normally camera is outside objects so we would cull back faces

            skyPipelineState.blending.enabled = false; // No blending needed for the sky.

            // Azon we allaho a3lam we are setting up the pipeline in the material class
            // skyPipelineState.setup(); // So previously we were just changing values in the C++ code
            //  // This func will actually interact with the API to change the configuration/state to the state/options we defined we'd like to use in the C++ values
            //  // You can think of it as all the previous changes were being made to RAM/CPU but this func applies those changes to the GPU

            // Load the sky texture (note that we don't need mipmaps since we want to avoid any unnecessary blurring while rendering the sky)
            std::string skyTextureFile = config.value<std::string>("sky", "");       // will return filename as string
            Texture2D *skyTexture = texture_utils::loadImage(skyTextureFile, false); // will turn that file to a 2D array of texture coordinates
            // will generate the sky text bas without mip-maps since "false"

            // Setup a sampler for the sky
            Sampler *skySampler = new Sampler();
            // kol el ta7t is (option,value we set to that option) pairs, refer to documentation if you forgot/need any of them
            skySampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR); // TODO: what does this do?
            skySampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_WRAP_S, GL_REPEAT);        // S is x axis
            skySampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // T is y axis
            // Note: For 3D textures, R is used for z axis
            // Repeats will reuse leftmost values post right-edge and vice versa
            // clamp to edge will use the value in the upper row and repeat that for any row above
            // it will do the same thing with bottom row/rows below it

            // Combine all the aforementioned objects (except the mesh) into a material
            this->skyMaterial = new TexturedMaterial();
            this->skyMaterial->shader = skyShader;   // the shader program that will map the texture vertices and retrieve their color
            this->skyMaterial->texture = skyTexture; // 2D array of (u,v)
            this->skyMaterial->sampler = skySampler; // specifying how we will use/map the 2D array above
            this->skyMaterial->pipelineState = skyPipelineState;
            this->skyMaterial->tint = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            this->skyMaterial->alphaThreshold = 1.0f;
            this->skyMaterial->transparent = false;
        }

        // Then we check if there is a postprocessing shader in the configuration
        if (config.contains("postprocess"))
        {
            // TODO: (Req 11) Create a framebuffer
            glGenFramebuffers(1, &postprocessFrameBuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, postprocessFrameBuffer);
            // TODO: (Req 11) Create a color and a depth texture and attach them to the framebuffer
            //  Hints: The color format can be (Red, Green, Blue and Alpha components with 8 bits for each channel).
            //  The depth format can be (Depth component with 24 bits).

            // colorTarget = new Texture2D(); // No need to bind the texture as it is already done in creating the class "constructer"
            //  colorTarget->bind();
            //  Create color texture
            colorTarget = texture_utils::empty(GL_RGBA8, windowSize, GL_RGBA);
            // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, windowSize.x, windowSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTarget->getOpenGLName(), 0);
            // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, windowSize.x, windowSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            // Create depth texture
            depthTarget = new Texture2D(); // No need to bind the texture as it is already done in creating the class "constructer"
            // depthTarget->bind();
            // depthTarget = texture_utils::empty(GL_DEPTH_COMPONENT24, windowSize);
            glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, windowSize.x, windowSize.y);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTarget->getOpenGLName(), 0);

            // TODO: (Req 11) Unbind the framebuffer just to be safe
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // Create a vertex array to use for drawing the texture
            glGenVertexArrays(1, &postProcessVertexArray);

            // Create a sampler to use for sampling the scene texture in the post processing shader
            Sampler *postprocessSampler = new Sampler();
            postprocessSampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            postprocessSampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            postprocessSampler->set(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            postprocessSampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Create the post processing shader
            ShaderProgram *postprocessShader = new ShaderProgram();
            postprocessShader->attach("assets/shaders/fullscreen.vert", GL_VERTEX_SHADER);
            postprocessShader->attach(config.value<std::string>("postprocess", ""), GL_FRAGMENT_SHADER);
            postprocessShader->link();

            // Create a post processing material
            postprocessMaterial = new TexturedMaterial();
            postprocessMaterial->shader = postprocessShader;
            postprocessMaterial->texture = colorTarget;
            postprocessMaterial->sampler = postprocessSampler;
            // The default options are fine but we don't need to interact with the depth buffer
            // so it is more performant to disable the depth mask
            postprocessMaterial->pipelineState.depthMask = false;
        }
    }

    void ForwardRenderer::destroy()
    {
        // Delete all objects related to the sky
        if (skyMaterial)
        {
            delete skySphere;
            delete skyMaterial->shader;
            delete skyMaterial->texture;
            delete skyMaterial->sampler;
            delete skyMaterial;
        }
        // Delete all objects related to post processing
        if (postprocessMaterial)
        {
            glDeleteFramebuffers(1, &postprocessFrameBuffer);
            glDeleteVertexArrays(1, &postProcessVertexArray);
            delete colorTarget;
            delete depthTarget;
            delete postprocessMaterial->sampler;
            delete postprocessMaterial->shader;
            delete postprocessMaterial;
        }
    }

    void ForwardRenderer::render(World *world)
    {
        // First of all, we search for a camera and for all the mesh renderers
        CameraComponent *camera = nullptr;
        lights.clear();
        opaqueCommands.clear();
        transparentCommands.clear();
        for (auto entity : world->getEntities())
        {
            // If we hadn't found a camera yet, we look for a camera in this entity
            if (!camera)
                camera = entity->getComponent<CameraComponent>();
            // If this entity has a mesh renderer component
            if (auto meshRenderer = entity->getComponent<MeshRendererComponent>(); meshRenderer)
            {
                // We construct a command from it
                RenderCommand command;
                command.localToWorld = meshRenderer->getOwner()->getLocalToWorldMatrix();
                command.center = glm::vec3(command.localToWorld * glm::vec4(0, 0, 0, 1));
                command.mesh = meshRenderer->mesh;
                command.material = meshRenderer->material;
                // if it is transparent, we add it to the transparent commands list
                if (command.material->transparent)
                {
                    transparentCommands.push_back(command);
                }
                else
                {
                    // Otherwise, we add it to the opaque command list
                    opaqueCommands.push_back(command);
                }
            }
            // If this entity has a light component
            if (auto light = entity->getComponent<LightComponent>(); light)
            {
                // We add it to the list of lights
                lights.push_back(light);
            }
        }

        // If there is no camera, we return (we cannot render without a camera)
        if (camera == nullptr)
            return;

        // TODO: (Req 9) Modify the following line such that "cameraForward" contains a vector pointing the camera forward direction
        //  HINT: See how you wrote the CameraComponent::getViewMatrix, it should help you solve this one
        glm::vec3 cameraForward = glm::normalize(glm::vec3(camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(0, 0, -1, 0)));
        auto owner = camera->getOwner();
        auto M = owner->getLocalToWorldMatrix();
        glm::vec3 eye = M * glm::vec4(0, 0, 0, 1);
        // Req 9: Sorting transparent objects
        std::sort(transparentCommands.begin(), transparentCommands.end(), [cameraForward](const RenderCommand &first, const RenderCommand &second)
                  {
            //TODO: (Req 9) Finish this function
            // HINT: the following return should return true "first" should be drawn before "second".
            return glm::dot(cameraForward, first.center) > glm::dot(cameraForward, second.center);
            return false; });

        // std::sort(transparentCommands.begin(), transparentCommands.end(), [cameraForward](const RenderCommand &first, const RenderCommand &second) { // HINT: the following return should return true "first" should be drawn before "second".
        //     return glm::dot(cameraForward, first.center) > glm::dot(cameraForward, second.center);
        // });

        // Req 9: Additional camera and rendering setup (VP matrix, viewport, clear color/depth)
        // glm::mat4 VP = camera->getProjectionMatrix(windowSize) * camera->getViewMatrix();
        // glViewport(0, 0, windowSize.x, windowSize.y);
        // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        // glClearDepth(1.0f);

        // glColorMask(true, true, true, true); // I'm not convinced azon those should be set by pipeline state
        // glDepthMask(true);

        // TODO: (Req 9) Get the camera ViewProjection matrix and store it in VP
        glm::mat4 VP = camera->getProjectionMatrix(windowSize) * camera->getViewMatrix();

        // TODO: (Req 9) Set the OpenGL viewport using viewportStart and viewportSize
        glViewport(0, 0, windowSize.x, windowSize.y);

        // TODO: (Req 9) Set the clear color to black and the clear depth to 1
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClearDepth(1.0f);

        // TODO: (Req 9) Set the color mask to true and the depth mask to true (to ensure the glClear will affect the framebuffer)
        glColorMask(true, true, true, true); // I'm not convinced azon those should be set by pipeline state
        glDepthMask(true);

        // If there is a postprocess material, bind the framebuffer
        if (postprocessMaterial)
        {
            // TODO: (Req 11) bind the framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, postprocessFrameBuffer);
        }

        // TODO: (Req 9) Clear the color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // TODO: (Req 9) Draw all the opaque commands
        //  Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command
        // Draw Opaque Commands
        for (auto &command : opaqueCommands)
        {
            // command.material->shader->use();
            command.material->setup();
            // if object's material is lit
            if (auto light_material = dynamic_cast<LitMaterial *>(command.material); light_material)
            {
                // set all uniforms of vertex shader
                light_material->shader->set("VP", VP);
                light_material->shader->set("M", command.localToWorld);
                light_material->shader->set("eye", eye);
                light_material->shader->set("M_IT", glm::transpose(glm::inverse(command.localToWorld)));
                light_material->shader->set("light_count", (int)lights.size());

                // loop through the lights vector
                for (int i = 0; i < (int)lights.size(); i++)
                {
                    light_material->shader->set("lights[" + to_string(i) + "].type", static_cast<int>(lights[i]->lightType));

                    light_material->shader->set("lights[" + to_string(i) + "].color", lights[i]->color);

                    // calculate position and direction of the light source from its owner
                    glm::vec3 position = lights[i]->getOwner()->getLocalToWorldMatrix() * glm::vec4(0, 0, 0, 1);
                    glm::vec3 direction = lights[i]->getOwner()->getLocalToWorldMatrix() * glm::vec4(0, 0, -1, 0);
                    // glm::vec3 direction = lights[i]->getOwner()->getLocalToWorldMatrix();

                    switch (lights[i]->lightType)
                    {
                    case LightType::DIRECTIONAL:
                        light_material->shader->set("lights[" + to_string(i) + "].direction", direction);
                        break;
                    case LightType::POINT:
                        light_material->shader->set("lights[" + to_string(i) + "].position", position);
                        light_material->shader->set("lights[" + to_string(i) + "].attenuation_constant", lights[i]->attenuation[0]);
                        light_material->shader->set("lights[" + to_string(i) + "].attenuation_linear", lights[i]->attenuation[1]);
                        light_material->shader->set("lights[" + to_string(i) + "].attenuation_quadratic", lights[i]->attenuation[2]);
                        break;
                    case LightType::SPOT:
                        light_material->shader->set("lights[" + to_string(i) + "].position", position);
                        light_material->shader->set("lights[" + to_string(i) + "].direction", direction);
                        light_material->shader->set("lights[" + to_string(i) + "].attenuation_constant", lights[i]->attenuation[0]);
                        light_material->shader->set("lights[" + to_string(i) + "].attenuation_linear", lights[i]->attenuation[1]);
                        light_material->shader->set("lights[" + to_string(i) + "].attenuation_quadratic", lights[i]->attenuation[2]);
                        light_material->shader->set("lights[" + to_string(i) + "].inner_angle", lights[i]->cone_angles.x);
                        light_material->shader->set("lights[" + to_string(i) + "].outer_angle", lights[i]->cone_angles.y);
                        // printf("Inner Cone Angle: %f \n", lights[i]->cone_angles.x);
                        // printf("Outer Cone Angle: %f \n", lights[i]->cone_angles.y);
                        // printf("Inner Cone Angle 2: %f \n", lights[i]->cone_angles[0]);
                        // printf("Outer Cone Angle 2: %f \n", lights[i]->cone_angles[1]);

                        break;
                    }

                    // light_material->shader->set("lights[" + to_string(i) + "].diffuse", lights[i]->diffuse);
                    // light_material->shader->set("lights[" + to_string(i) + "].specular", lights[i]->specular);
                }
            }
            else
            {
                // set transform to be equal the model view projection matrix for each render command
                command.material->shader->set("transform", VP * command.localToWorld);
            }

            command.mesh->draw();
        }

        // If there is a sky material, draw the sky
        if (this->skyMaterial)
        {
            // TODO: (Req 10) setup the sky material
            skyMaterial->setup();
            // glDepthMask(GL_FALSE);
            //  TODO: (Req 10) Get the camera position
            //  cglDepthFunc(GL_LEQUAL); // Make sure sky is rendered in the background

            // TODO: (Req 10) Create a model matrix for the sy such that it always follows the camera (sky sphere center = camera position)
            glm::mat4 cameraMatrix = camera->getOwner()->getLocalToWorldMatrix();
            glm::vec3 cameraPosition = glm::vec3(cameraMatrix[3]);                   // 4th column in camera matrix is the translation vector
            glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), cameraPosition); // this will translate the sphere to the camera
            // TODO: (Req 10) We want the sky to be drawn behind everything (in NDC space, z=1)
            //  We can acheive the is by multiplying by an extra matrix after the projection but what values should we put in it?
            glm::mat4 alwaysBehindTransform = glm::mat4(
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 1.0f); // after dividing by w our z value will be 1 in NDC which is the furthest value
            glm::mat4 transform = alwaysBehindTransform * VP * modelMatrix;
            // TODO: (Req 10) set the "transform" uniform
            skyMaterial->shader->set("transform", transform);
            // TODO: (Req 10) draw the sky sphere
            skySphere->draw();
        }
        // TODO: (Req 9) Draw all the transparent commands
        //  Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command
        for (auto &command : transparentCommands)
        {
            // command.material->shader->use();
            command.material->setup();
            command.material->shader->set("transform", VP * command.localToWorld);
            command.mesh->draw();
        }
        // If there is a postprocess material, apply postprocessing
        if (postprocessMaterial)
        {
            // TODO: (Req 11) Return to the default framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            postprocessMaterial->setup();
            // postprocessMaterial->shader->use();
            glBindVertexArray(postProcessVertexArray);
            glDrawArrays(GL_TRIANGLES, 0, 3); // Assuming a fullscreen triangle
            glBindVertexArray(0);
            // TODO: (Req 11) Setup the postprocess material and draw the fullscreen triangle
        }

        if (MaterialLight)
        {
            MaterialLight->setup();
        }
    }
}