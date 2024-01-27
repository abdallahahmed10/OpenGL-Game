#include "texture-utils.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <iostream>

our::Texture2D *our::texture_utils::empty(GLenum format, glm::ivec2 size,GLenum format2)
{
    our::Texture2D *texture = new our::Texture2D();
    // TODO: (Req 11) Finish this function to create an empty texture with the given size and format

    // Bind the texture to set its parameters and allocate storage
    // texture->bind();

    // Allocate storage for the texture without initializing with data
    glTexImage2D(GL_TEXTURE_2D, 0, format, size.x, size.y, 0, format2, GL_UNSIGNED_BYTE, nullptr);
    // nullptr cuz we're not sending it any data, it'll be "empty"

    // Function Params:
    // target: Specifies the target texture, and in this case, it is typically GL_TEXTURE_2D for a 2D texture.
    // level: Specifies the level-of-detail number. For the base level, this is usually set to 0.
    // internalformat: Specifies the internal format of the image data on the GPU.
    // width and height: Specify the dimensions of the texture.
    // border: Specifies the width of the border. It is deprecated and should be set to 0.
    // format and type: Specify the format and data type of the pixel data being passed.
    // pixels: Points to the actual image data that will be uploaded to the GPU.

    // Unbind the texture
    //  texture->unbind();

    return texture;
}

our::Texture2D *our::texture_utils::loadImage(const std::string &filename, bool generate_mipmap)
{
    glm::ivec2 size;
    int channels;
    // Since OpenGL puts the texture origin at the bottom left while images typically has the origin at the top left,
    // We need to till stb to flip images vertically after loading them
    stbi_set_flip_vertically_on_load(true);
    // Load image data and retrieve width, height and number of channels in the image
    // The last argument is the number of channels we want and it can have the following values:
    //- 0: Keep number of channels the same as in the image file
    //- 1: Grayscale only
    //- 2: Grayscale and Alpha
    //- 3: RGB
    //- 4: RGB and Alpha (RGBA)
    // Note: channels (the 4th argument) always returns the original number of channels in the file

    // stbi_load: This is a function from the STB Image library, which is a simple image loading library for C and C++.

    // filename.c_str(): This part converts the C++ string filename to a C-style string (const char*) using the c_str() method. The stbi_load function expects a C-style string representing the filename of the image.

    // &size.x, &size.y: These are pointers to variables that will receive the width and height of the loaded image, respectively. The stbi_load function updates these variables with the dimensions of the loaded image.

    // &channels: This is a pointer to a variable that will receive the number of color channels in the loaded image. The stbi_load function updates this variable with the number of channels (e.g., 3 for RGB, 4 for RGBA).

    // 4: This parameter specifies the desired number of channels in the loaded image. In this case, 4 indicates that you want to force the loaded image to have four channels, even if the original image has a different number of channels. This is commonly used to ensure that the loaded image has an alpha channel, which is often needed in graphics programming.

    // unsigned char *pixels: This is a pointer to the buffer that will hold the pixel data of the loaded image. The stbi_load function allocates memory for the image data and updates the pixels pointer to point to this allocated memory.

    // After this line of code, the pixels pointer will point to the raw pixel data of the loaded image, and size.x, size.y, and channels will contain the dimensions and channel information of the image.

    unsigned char *pixels = stbi_load(filename.c_str(), &size.x, &size.y, &channels, 4);
    if (pixels == nullptr)
    {
        std::cerr << "Failed to load image: " << filename << std::endl;
        return nullptr;
    }
    // Create a texture
    our::Texture2D *texture = new our::Texture2D();
    // Bind the texture such that we upload the image data to its storage
    // TODO: (Req 5) Finish this function to fill the texture with the data found in "pixels"
    //  Fill the texture with data
    // texture->bind();
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    // uploads the pixels of the image to the texture in the GPU
    // define the storage and format of an image for a 2D texture in OpenGL
    // here upload from CPU to GPU occurs

    // Function Params:
    // target: Specifies the target texture, and in this case, it is typically GL_TEXTURE_2D for a 2D texture.
    // level: Specifies the level-of-detail number. For the base level, this is usually set to 0.
    // internalformat: Specifies the internal format of the image data on the GPU.
    // width and height: Specify the dimensions of the texture.
    // border: Specifies the width of the border. It is deprecated and should be set to 0.
    // format and type: Specify the format and data type of the pixel data being passed.
    // pixels: Points to the actual image data that will be uploaded to the GPU.

    // Instructs GPU to generate/store mipmaps for better texture quality
    glGenerateMipmap(GL_TEXTURE_2D);

    // texture->unbind();
    stbi_image_free(pixels); // Free image data after uploading to GPU
    return texture;
}