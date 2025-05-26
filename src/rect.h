#pragma once

typedef struct CFXRect CFXRect;
/**
 * @struct CFXRect
 * @brief Represents a rectangle defined by its top-left corner (x, y) and its width (w) and height (h).
 *
 * This structure is commonly used for defining rectangular areas in 2D space,
 * such as bounding boxes, viewports, or regions of interest.
 *
 * Members:
 *   int x - The x-coordinate of the top-left corner of the rectangle.
 *   int y - The y-coordinate of the top-left corner of the rectangle.
 *   int w - The width of the rectangle.
 *   int h - The height of the rectangle.
 */
struct CFXRect {
    int x;
    int y;
    int w;
    int h;
};
