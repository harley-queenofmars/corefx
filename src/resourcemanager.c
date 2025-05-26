#include <stdio.h>
#include <string.h>
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <corefw.h>       // IWYU pragma: keep
#include "corefx.h"                 // IWYU pragma: keep
#include "shader.h"
#include "texture2d.h"
#include <stb_image.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
 
class2(CFXResourceManager);

void Init(CFXResourceManagerRef this);

CFXShaderRef LoadShaderFromFile(
    const CFXResourceManagerRef this,
    const GLchar* vShaderFile,
    const GLchar* fShaderFile);
CFXTexture2DRef LoadTextureFromFile(
    const CFXResourceManagerRef this,
    const GLchar* file,
    GLboolean alpha);

/**
 * @brief Destructor function for the CFXResourceManager object.
 *
 * This function releases all resources managed by the resource manager,
 * including shaders and textures. It iterates through the internal maps
 * of shaders and textures, unreferences each contained object if it matches
 * the expected class type, and finally unreferences the maps themselves.
 *
 * @param self Pointer to the CFXResourceManager instance to be destroyed.
 */
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

/**
 * @brief Initializes the resource manager by creating new maps for shaders and textures.
 *
 * This function allocates and assigns new CFMap instances to the Shaders and Textures
 * members of the resource manager. These maps are intended to store shader and texture
 * resources, respectively.
 *
 * @param this Pointer to the CFXResourceManagerRef instance to initialize.
 */
void Init(CFXResourceManagerRef this)
{
    this->Shaders = CFNew(CFMap, nullptr);
    this->Textures = CFNew(CFMap, nullptr);
}

/**
 * @brief Constructor for the CFXResourceManager object.
 *
 * Initializes the given CFXResourceManagerRef instance and returns it.
 *
 * @param this Pointer to the CFXResourceManagerRef instance to initialize.
 * @return void* Pointer to the initialized CFXResourceManagerRef instance.
 */
proc void* Ctor(CFXResourceManagerRef this)
{
    Init(this);
    return this;
}

/**
 * @brief Loads a shader from the specified vertex and fragment shader files and stores it in the resource manager.
 *
 * This function loads a shader using the provided vertex and fragment shader file paths,
 * associates it with the given name in the resource manager's shader map, and returns a reference to the loaded shader.
 *
 * @param this         Reference to the resource manager.
 * @param vShaderFile  Path to the vertex shader file.
 * @param fShaderFile  Path to the fragment shader file.
 * @param name         Name to associate with the loaded shader in the resource manager.
 * @return             Reference to the loaded shader.
 */
proc CFXShaderRef LoadShader(
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
 * Retrieves a shader resource by its name from the resource manager.
 *
 * @param this  A reference to the resource manager instance.
 * @param name  The name of the shader to retrieve.
 * @return      A reference to the shader if found; otherwise, NULL.
 */
proc CFXShaderRef GetShader(
    const CFXResourceManagerRef this,
    const char* name)
{
    return CFMapGetC(this->Shaders, name);
}

/**
 * Loads a texture from a file and stores it in the resource manager's texture map.
 *
 * @param this   Reference to the resource manager.
 * @param file   Path to the texture file to load.
 * @param alpha  Specifies whether the texture should include an alpha channel.
 * @param name   Name to associate with the loaded texture in the texture map.
 * @return       Reference to the loaded texture.
 */
proc CFXTexture2DRef LoadTexture(
    const CFXResourceManagerRef this,
    const GLchar* file,
    GLboolean alpha,
    const char* name)
{
    CFMapSetC(this->Textures, name, LoadTextureFromFile(this, file, alpha));
    return CFMapGetC(this->Textures, name);
}

/**
 * Retrieves a texture resource by its name from the resource manager.
 *
 * @param this Pointer to the resource manager instance.
 * @param name The name of the texture to retrieve.
 * @return A reference to the CFXTexture2D resource if found, otherwise NULL.
 */
proc CFXTexture2DRef GetTexture(
    const CFXResourceManagerRef this,
    const char* name)
{
    return CFMapGetC(this->Textures, name);
}

/**
 * @brief Clears the resource manager by destructing and reinitializing it.
 *
 * This function first calls the destructor on the given resource manager instance
 * to release any allocated resources, and then reinitializes it to a clean state.
 *
 * @param this A reference to the resource manager to be cleared.
 */
void Clear(CFXResourceManagerRef this)
{
    dtor(this);
    Init(this);
}

/**
 * @brief Loads a shader from vertex and fragment shader source files.
 *
 * This function reads the contents of the specified vertex and fragment shader files,
 * then creates and returns a new shader object using those sources.
 *
 * @param this         Reference to the resource manager.
 * @param vShaderFile  Path to the vertex shader source file.
 * @param fShaderFile  Path to the fragment shader source file.
 * @return             Reference to the created shader object.
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
 * @brief Loads a texture from a file and creates a CFXTexture2DRef object.
 *
 * This function loads an image file using stb_image, optionally with an alpha channel,
 * and creates a texture object suitable for use with OpenGL. The image is flipped
 * vertically during loading to match OpenGL's coordinate system.
 *
 * @param this      The resource manager reference (unused in this function).
 * @param file      The path to the image file to load.
 * @param alpha     GL_TRUE to load the image with an alpha channel (RGBA), GL_FALSE for RGB only.
 * @return          A reference to the created CFXTexture2D object containing the loaded texture.
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
