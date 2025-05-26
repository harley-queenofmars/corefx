#pragma once
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include "tglm.h"               // IWYU pragma: keep
#include <corefw.h>   // IWYU pragma: keep

extern CFClassRef CFXTexture2D;

typedef struct __CFXTexture2D* CFXTexture2DRef;

/**
 * @struct __CFXTexture2D
 * @brief Represents a 2D texture object in the CoreFW framework.
 *
 * This structure encapsulates all the necessary information for managing a 2D texture,
 * including its OpenGL identifier, dimensions, formats, wrapping and filtering modes, and file path.
 *
 * Members:
 * - obj: CoreFW interface object for resource management.
 * - Id: OpenGL texture object ID, used to reference this texture in OpenGL operations.
 * - Width: Width of the loaded image in pixels.
 * - Height: Height of the loaded image in pixels.
 * - InternalFormat: Format specification for the texture object in OpenGL.
 * - ImageFormat: Format of the loaded image data.
 * - wrapS: Wrapping mode for the S (horizontal) axis.
 * - wrapT: Wrapping mode for the T (vertical) axis.
 * - filterMin: Filtering mode when texture pixels are smaller than screen pixels (minification).
 * - filterMag: Filtering mode when texture pixels are larger than screen pixels (magnification).
 * - path: File path to the loaded image.
 */
typedef struct __CFXTexture2D {
    __CFObject obj;         // CoreFW interface
    GLuint Id;              // Holds the ID of the texture object, used for all texture operations to reference to self particlar texture
    GLuint Width, Height;   // Width and height of loaded image in pixels
    GLuint InternalFormat;  // Format of texture object
    GLuint ImageFormat;     // Format of loaded image
    GLuint wrapS;           // Wrapping mode on S axis
    GLuint wrapT;           // Wrapping mode on T axis
    GLuint filterMin;       // Filtering mode if texture pixels < screen pixels
    GLuint filterMag;       // Filtering mode if texture pixels > screen pixels
    char* path;
} __CFXTexture2D;

extern proc void* Ctor(
    CFXTexture2DRef this, 
    GLuint internalFormat, 
    GLuint imageFormat, 
    char* path);

extern proc void Generate(
    CFXTexture2DRef this,
    GLuint width,
    GLuint height,
    unsigned char* data);

extern proc void Bind(
    const CFXTexture2DRef this);

extern proc char* ToString(
    const CFXTexture2DRef this);

/**
 * @brief Creates a new CFXTexture2D object with the specified formats and image path.
 *
 * This function constructs a new CFXTexture2D instance by allocating memory and initializing it
 * with the provided internal format, image format, and file path to the texture image.
 *
 * @param internalFormat The OpenGL internal format to be used for the texture (e.g., GL_RGB, GL_RGBA).
 * @param imageFormat The OpenGL format of the loaded image data (e.g., GL_RGB, GL_RGBA).
 * @param path The file path to the image to be loaded as a texture.
 * @return CFXTexture2DRef A reference to the newly created CFXTexture2D object.
 */
static inline CFXTexture2DRef NewCFXTexture2D(GLuint internalFormat, GLuint imageFormat, char* path)
{
    return Ctor((CFXTexture2DRef)CFCreate(CFXTexture2D), internalFormat, imageFormat, path);
}
