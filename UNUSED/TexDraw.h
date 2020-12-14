#ifndef _TEXDRAW_H
#define _TEXDRAW_H

void DrawPolygon(TexInfo Model, Vec3 Position, Vec3 Scale, s16 Rotation, Vec3 Translation,
                 Vec2 TopRight, Vec2 TopLeft, Vec2 BottomLeft, Vec2 BottomRight, int Opacity, int BottomSkew);

#endif