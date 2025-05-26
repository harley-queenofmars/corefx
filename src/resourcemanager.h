#pragma once
#include <corefw.h>       // IWYU pragma: keep
#include "corefx.h"                 // IWYU pragma: keep
#include "shader.h"
#include "texture2d.h"

extern CFClassRef CFXResourceManager;

typedef struct __CFXResourceManager* CFXResourceManagerRef;

/**
 * @struct __CFXResourceManager
 * @brief Manages collections of various graphics resources.
 *
 * This structure encapsulates resource maps for shaders, textures, and fonts,
 * providing centralized management and lookup functionality for these assets.
 *
 * Members:
 * - obj:      Base object information for resource manager.
 * - Shaders:  Map reference holding shader resources.
 * - Textures: Map reference holding texture resources.
 * - Fonts:    Map reference holding font resources.
 */
typedef struct __CFXResourceManager {
    __CFObject obj;
    CFMapRef Shaders;
    CFMapRef Textures;
    CFMapRef Fonts;
} __CFXResourceManager;

extern proc void* Ctor(
    CFXResourceManagerRef this);

extern proc CFXShaderRef LoadShader(
    const CFXResourceManagerRef this,
    const GLchar* vShaderFile,
    const GLchar* fShaderFile,
    const char* name);

extern proc CFXShaderRef GetShader(
    const CFXResourceManagerRef this,
    const char* name);

extern proc CFXTexture2DRef LoadTexture(
    const CFXResourceManagerRef this,
    const GLchar* file,
    GLboolean alpha,
    const char* name);

extern proc CFXTexture2DRef GetTexture(
    const CFXResourceManagerRef this,
    const char* name);

/**
 * @brief Creates and initializes a new CFXResourceManager instance.
 *
 * This function allocates memory for a new CFXResourceManager object and
 * initializes it using the constructor. It returns a reference to the newly
 * created resource manager.
 *
 * @return CFXResourceManagerRef Reference to the newly created resource manager.
 */
static inline CFXResourceManagerRef NewCFXResourceManager()
{
    return Ctor((CFXResourceManagerRef)CFCreate(CFXResourceManager));
}
