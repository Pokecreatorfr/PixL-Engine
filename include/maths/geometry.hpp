#pragma once

inline bool isPointInRect(int x, int y, int rectX, int rectY, int rectW, int rectH);

// regarder si deux rectangles se superposent

inline bool isRectInRect(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);

// regarder si un rectangle est strictement dans un autre

inline bool isRectInRectStrict(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);