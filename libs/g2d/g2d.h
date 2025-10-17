/**
    Internal graphic functions of Imhotep Engine
    File - g2d.h
    *Made by Kodilo*
*/
#include <glib2d.h>

/**
    * \brief Draw an image on screen
    * @param tex g2d texture
    * @param x The X-Coordinate position
    * @param y The Y-Coordinate position
    * @param w Scale width
    * @param h Scale height
    * @param color Overlay color
    * @param srcx The X-Coordinate for crop
    * @param srcy The Y-Coordinate for crop
    * @param srcw Crop width
    * @param srch Crop height
    * @param r Rotate angle
    * @param a Transparency value
    * @param mode Coordination mode
*/
int g2D_draw(g2dImage tex, int x, int y, int w, int h, g2dColor color, int srcx, int srcy, int srcw, int srch, int r, int a, g2dCoord_Mode mode);

/**
    * \brief Fast draw an image on screen
    * @param tex g2d texture
    * @param x The X-Coordinate position
    * @param y The Y-Coordinate position
    * @param color Overlay color
    * @param r Rotate angle
    * @param a Transparency value
    * @param mode Coordination mode
*/
int g2D_draweasy(g2dImage tex, int x, int y, g2dColor color, int r, int a, g2dCoord_Mode mode);

/**
    * \brief Draw a rectangle
    * @param x The X-Coordinate position
    * @param y The Y-Coordinate position
    * @param w Reactangle width
    * @param h Rectangle height
    * @param color Rectangle color
    * @param r Rectangle rotate
    * @param a Rectangle transparency
*/
int g2D_drawRect(int x, int y, int w, int h, g2dColor color, int r, int a);

/**
 * \brief Draw a circle
 * @param x The X-Coordinate position
 * @param y The Y-Coordinate position
 * @param r Circle radius
 * @param color Circle color
 * @param a Circle transparency
*/
int g2D_drawCircle(int x, int y, int r, g2dColor color, int a);