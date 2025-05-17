#pragma once
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include "tglm.h"               // IWYU pragma: keep
#include "../corefw/corefw.h"   // IWYU pragma: keep

extern CFClassRef CFXTexture2D;

typedef struct __CFXTexture2D* CFXTexture2DRef;
/**
 * class CFXTexture2D
 */
typedef struct __CFXTexture2D {
    __CFObject obj; // CoreFW interface
    GLuint Id; // Holds the ID of the texture object, used for all texture operations to reference to self particlar texture
    GLuint Width, Height; // Width and height of loaded image in pixels
    GLuint InternalFormat; // Format of texture object
    GLuint ImageFormat; // Format of loaded image
    GLuint wrapS; // Wrapping mode on S axis
    GLuint wrapT; // Wrapping mode on T axis
    GLuint filterMin; // Filtering mode if texture pixels < screen pixels
    GLuint filterMag; // Filtering mode if texture pixels > screen pixels
    char* path;
} __CFXTexture2D;

extern method void* Ctor(
    CFXTexture2DRef this, 
    GLuint internalFormat, 
    GLuint imageFormat, 
    char* path);

extern method void Generate(
    CFXTexture2DRef this,
    GLuint width,
    GLuint height,
    unsigned char* data);

extern method void Bind(
    const CFXTexture2DRef this);

extern method char* ToString(
    const CFXTexture2DRef this);

static inline CFXTexture2DRef NewCFXTexture2D(GLuint internalFormat, GLuint imageFormat, char* path)
{
    return Ctor((CFXTexture2DRef)CFCreate(CFXTexture2D), internalFormat, imageFormat, path);
}
