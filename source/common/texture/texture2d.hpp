#pragma once

#include <glad/gl.h>

namespace our {

    // This class defined an OpenGL texture which will be used as a GL_TEXTURE_2D
    class Texture2D {
        // The OpenGL object name of this texture 
        GLuint name = 0;
    public:
        // This constructor creates an OpenGL texture and saves its object name in the member variable "name" 
        Texture2D() {
            //TODO: (Req 5) Complete this function
           
            /*
            The glGenTextures function first takes as input how many textures we want to generate
            and stores them in a unsigned int array given as its second argument (in our case just a single
            unsigned int). Just like other objects we need to bind it so any subsequent texture commands
            will configure the currently bound texture:
            */
            glGenTextures(1, &name);
            bind();
            // set the texture wrapping/filtering options (on currently bound texture)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
            // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);

            // glBindTexture(GL_TEXTURE_2D, name);
        };

        // This deconstructor deletes the underlying OpenGL texture
        ~Texture2D() { 
            //TODO: (Req 5) Complete this function
            unbind();
            glDeleteTextures(1, &name);
        }

        // Get the internal OpenGL name of the texture which is useful for use with framebuffers
        GLuint getOpenGLName() {
            return name;
        }

        // This method binds this texture to GL_TEXTURE_2D
        void bind() const {
            //TODO: (Req 5) Complete this function
            // glBindTexture: This is an OpenGL function used to bind a texture to a texture target.

            // GL_TEXTURE_2D: This specifies the target texture type, indicating that you are binding a 2D texture.

            // name: This is the ID (name) of the texture you want to bind. In OpenGL, when you generate or create a texture, it is assigned a unique identifier, and this identifier is used in subsequent operations to refer to the texture.

            // Putting it all together, the line is saying, "Bind the 2D texture with the identifier name to the GL_TEXTURE_2D texture target."
            glBindTexture(GL_TEXTURE_2D, name);
        }

        // This static method ensures that no texture is bound to GL_TEXTURE_2D
        static void unbind(){
            //TODO: (Req 5) Complete this function
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        Texture2D(const Texture2D&) = delete;
        Texture2D& operator=(const Texture2D&) = delete;
    };
    
}