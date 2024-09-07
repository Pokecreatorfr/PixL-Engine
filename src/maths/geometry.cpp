#include <maths/geometry.hpp>

inline bool isPointInRect(int x, int y, int rectX, int rectY, int rectW, int rectH)
{
    return x >= rectX && x <= rectX + rectW && y >= rectY && y <= rectY + rectH;
}

inline bool isRectInRect(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
    return x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2;
}

inline bool isRectInRectStrict(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
    return x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2 && (x1 != x2 || y1 != y2 || w1 != w2 || h1 != h2);
}