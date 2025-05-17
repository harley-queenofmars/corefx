#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include "../corefw/corefw.h"    // IWYU pragma: keep
#include "stb_image.h"
#include <GLFW/glfw3.h>
#include "texture2d.h"

class(CFXTexture2D);

/**
 *  CFXTexture2D Constructor
 * 
 * @param internalFormat
 * @param imageFormat
 * @param path to image
 */
method void* Ctor(CFXTexture2DRef this, GLuint internalFormat, GLuint imageFormat, char* path)
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
 * Generate
 * 
 * @param width of image to generate
 * @param height of image to generate
 * @param data bitmap data
 * 
 */
method void Generate(
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

/**
 * Bind
 * 
 * binds the texture to GL
 */
method void Bind(const CFXTexture2DRef this)
{
    glBindTexture(GL_TEXTURE_2D, this->Id);
}

method char* ToString(const CFXTexture2DRef this)
{
    char* s = calloc(1024, 1);
    return s;
}
