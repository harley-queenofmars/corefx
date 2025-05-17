#include <stdio.h>
#include <string.h>
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../corefw/corefw.h"       // IWYU pragma: keep
#include "corefx.h"                 // IWYU pragma: keep
#include "shader.h"
#include "texture2d.h"
#include <stb_image.h>

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
 
class2(CFXResourceManager, dtor);

void Init(CFXResourceManagerRef this);

CFXShaderRef LoadShaderFromFile(
    const CFXResourceManagerRef this,
    const GLchar* vShaderFile,
    const GLchar* fShaderFile);
CFXTexture2DRef LoadTextureFromFile(
    const CFXResourceManagerRef this,
    const GLchar* file,
    GLboolean alpha);

static void dtor(void* self)
{
    CFXResourceManagerRef this = self;
    CFMapIter_t iter;

    CFMapIter(this->Shaders, &iter);
    while (iter.key != nullptr) {
        if (CFIs(iter.obj, (CFClassRef)CFXShader))
            CFUnref(iter.obj);
        CFMapIterNext(&iter);
    }
    CFUnref(this->Shaders);

    CFMapIter(this->Textures, &iter);
    while (iter.key != nullptr) {
        if (CFIs(iter.obj, (CFClassRef)CFXTexture2D))
            CFUnref(iter.obj);
        CFMapIterNext(&iter);
    }
    CFUnref(this->Textures);
}

void Init(CFXResourceManagerRef this)
{
    this->Shaders = CFNew(CFMap, nullptr);
    this->Textures = CFNew(CFMap, nullptr);
}
method void* Ctor(CFXResourceManagerRef this)
{
    Init(this);
    return this;
}
/**
 * LoadShader
 * 
 * @param vShaderFile path to vertex shader
 * @param fShaderFile path to fragment shader
 * @param name to cache as
 * @returns loaded, compiled and linked shader program
 */
method CFXShaderRef LoadShader(
    const CFXResourceManagerRef this,
    const GLchar* vShaderFile,
    const GLchar* fShaderFile,
    const char* name)
{
    assert(this != nullptr);

    CFMapSetC(this->Shaders, name, LoadShaderFromFile(this, vShaderFile, fShaderFile));
    return CFMapGetC(this->Shaders, name);
}

/**
 * GetShader
 * 
 * @param name to retrieve
 * @returns loaded, compiled and linked shader program
 * 
 */
method CFXShaderRef GetShader(
    const CFXResourceManagerRef this,
    const char* name)
{
    return CFMapGetC(this->Shaders, name);
}

/**
 * loadTexture
 * 
 * @param file path to texture
 * @param alpha does the texture have an alpha channel?
 * @param name to cache as
 * @returns Texture
 * 
 */
method CFXTexture2DRef LoadTexture(
    const CFXResourceManagerRef this,
    const GLchar* file,
    GLboolean alpha,
    const char* name)
{
    CFMapSetC(this->Textures, name, LoadTextureFromFile(this, file, alpha));
    return CFMapGetC(this->Textures, name);
}

/**
 * GetTexture
 * 
 * @param name to retrieve
 * @returns Texture
 * 
 */
method CFXTexture2DRef GetTexture(
    const CFXResourceManagerRef this,
    const char* name)
{
    return CFMapGetC(this->Textures, name);
}

void Clear(CFXResourceManagerRef this)
{
    dtor(this);
    Init(this);
}

// static inline char* join(const char* s1, const char* s2) { return nullptr;}
/**
 * loadShaderFromFile
 * 
 * @param vShaderFile path to vertex shader
 * @param fShaderFile path to fragment shader
 * @returns loaded, compiled and linked shader program
 * 
 */


CFXShaderRef LoadShaderFromFile(
    const CFXResourceManagerRef this,
    const GLchar* vShaderFile,
    const GLchar* fShaderFile)
{

    CFStringRef vShader = CFFS.readTextFile((char*)vShaderFile);
    CFStringRef fShader = CFFS.readTextFile((char*)fShaderFile);

    return Ctor((CFXShaderRef)CFCreate(CFXShader), vShader, fShader);
}

/**
 * loadTextureFromFile
 * 
 * @param file path to texture
 * @param alpha does the texture have an alpha channel?
 * @returns Texture
 * 
 */
CFXTexture2DRef LoadTextureFromFile(
    const CFXResourceManagerRef this,
    const GLchar* file,
    GLboolean alpha)
{
    int format = alpha ? GL_RGBA : GL_RGB;
    int stbiFlag = alpha ? STBI_rgb_alpha : STBI_rgb;

    CFXTexture2DRef texture = Ctor((CFXTexture2DRef)CFCreate(CFXTexture2D), format, format, (char*)file);

    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    int width, height, nrChannels;
    unsigned char* data = stbi_load(file, &width, &height, &nrChannels, stbiFlag);
    Generate(texture, width, height, (unsigned char*)data);
    stbi_image_free(data);

    return texture;
}
