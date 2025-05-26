#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <corefw.h>    // IWYU pragma: keep
#include "stb_image.h"
#include <GLFW/glfw3.h>
#include "texture2d.h"

class(CFXTexture2D);

/**
 * @brief Constructor for the CFXTexture2D object.
 *
 * Initializes a new 2D texture object with the specified internal and image formats,
 * sets default texture parameters (wrapping and filtering), duplicates the provided path,
 * and generates an OpenGL texture ID.
 *
 * @param this           Pointer to the CFXTexture2D object to initialize.
 * @param internalFormat OpenGL internal format for the texture (e.g., GL_RGB, GL_RGBA).
 * @param imageFormat    OpenGL image format for the texture data.
 * @param path           File path to the texture image (will be duplicated and stored).
 * @return               Pointer to the initialized CFXTexture2D object.
 */
proc void* Ctor(CFXTexture2DRef this, GLuint internalFormat, GLuint imageFormat, char* path)
{
    // CFXTexture2DRef this = CFNew((CFClassRef)CFXTexture2D);
    this->path = CFStrDup(path);
    this->Width = 0;
    this->Height = 0;
    this->wrapS = GL_REPEAT;
    this->wrapT = GL_REPEAT;
    this->filterMin = GL_LINEAR;
    this->filterMag = GL_LINEAR;
    this->InternalFormat = internalFormat;
    this->ImageFormat = imageFormat;
    glGenTextures(1, &this->Id);
    return this;
}

/**
 * @brief Generates and configures a 2D texture object.
 *
 * This function initializes a 2D texture with the specified width, height, and pixel data.
 * It binds the texture, uploads the image data to the GPU, and sets the texture's wrapping
 * and filtering parameters according to the object's properties. Finally, it unbinds the texture.
 *
 * @param this   Reference to the texture object to be generated and configured.
 * @param width  Width of the texture in pixels.
 * @param height Height of the texture in pixels.
 * @param data   Pointer to the image data to be uploaded to the texture.
 */
proc void Generate(
    CFXTexture2DRef this,
    GLuint width,
    GLuint height,
    unsigned char* data)
{
    this->Width = width;
    this->Height = height;
    // Create Texture
    glBindTexture(GL_TEXTURE_2D, this->Id);
    glTexImage2D(GL_TEXTURE_2D, 0, this->InternalFormat, width, height, 0, this->ImageFormat, GL_UNSIGNED_BYTE, data);
    // Set Texture wrap and filter modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->filterMin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->filterMag);
    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

proc void Bind(const CFXTexture2DRef this)
{
    glBindTexture(GL_TEXTURE_2D, this->Id);
}

proc char* ToString(const CFXTexture2DRef this)
{
    char* s = calloc(1024, 1);
    return s;
}
