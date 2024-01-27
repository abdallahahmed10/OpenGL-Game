#pragma once

#include <glad/gl.h>
#include <json/json.hpp>
#include <glm/vec4.hpp>

namespace our {

    // This class defined an OpenGL sampler
    class Sampler {
        // The OpenGL object name of this sampler 
        GLuint name;
    public:
        // This constructor creates an OpenGL sampler and saves its object name in the member variable "name" 
        Sampler() {
            //TODO: (Req 6) Complete this function
            glGenSamplers(1, &name);
        };

        // This deconstructor deletes the underlying OpenGL sampler
        ~Sampler() { 
            //TODO: (Req 6) Complete this function
            glDeleteSamplers(1, &name);
         }

        // This method binds this sampler to the given texture unit
        void bind(GLuint textureUnit) const {
            //TODO: (Req 6) Complete this function
            // glBindSampler: This is an OpenGL function used to bind a sampler object to a texture unit.

            // textureUnit: This is the texture unit to which the sampler object will be bound. Texture units are part of the programmable pipeline in OpenGL and are used to manage multiple textures in a shader.

            // name: This is the name (ID) of the sampler object that you want to bind. In OpenGL, sampler objects are used to control how texture sampling is performed, such as filtering and addressing modes.
            glBindSampler(textureUnit, name);
        }

        // This static method ensures that no sampler is bound to the given texture unit
        static void unbind(GLuint textureUnit){
            //TODO: (Req 6) Complete this function
            glBindSampler(textureUnit, 0);
        }

        // This function sets a sampler paramter where the value is of type "GLint"
        // This can be used to set the filtering and wrapping parameters
        void set(GLenum parameter, GLint value) const {
            //TODO: (Req 6) Complete this function
            //  glSamplerParameteri: This is an OpenGL function used to set integer parameters for a sampler object.

            // name: This is the name (ID) of the sampler object for which you want to set a parameter.

            // parameter: This is the symbolic name of the parameter you want to set. It could be a constant such as GL_TEXTURE_MIN_FILTER, GL_TEXTURE_MAG_FILTER, GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T, etc. The specific parameter you choose determines the aspect of the sampler's behavior that you are configuring.

            // value: This is the integer value you want to set for the specified parameter.
            glSamplerParameteri(name, parameter, value);
        }

        // This function sets a sampler paramter where the value is of type "GLfloat"
        // This can be used to set the "GL_TEXTURE_MAX_ANISOTROPY_EXT" parameter
        void set(GLenum parameter, GLfloat value) const {
            //TODO: (Req 6) Complete this function
            glSamplerParameteri(name, parameter, value);
        }

        // This function sets a sampler paramter where the value is of type "GLfloat[4]"
        // This can be used to set the "GL_TEXTURE_BORDER_COLOR" parameter
        void set(GLenum parameter, glm::vec4 value) const {
            glSamplerParameterfv(name, parameter, &(value.r));
        }

        // Given a json object, this function deserializes the sampler state
        void deserialize(const nlohmann::json& data);

        Sampler(const Sampler&) = delete;
        Sampler& operator=(const Sampler&) = delete;
    };

}