#include "game.h"
#include "registers.h"
#include "TexDraw.h"

static inline void vec3(int x, int y, int z)
    {
        REG_VERTEX16 = (y << 16) | (x & 0xFFFF);
        REG_VERTEX16 = (z & 0xFFFF);
    }

static inline void texcoord2(int x, int y)
    {
        REG_TEXT_COORD = (y << 16) | (x & 0xFFFF);
    }

void G3_LoadMtx43(u32 unk);

void DrawPolygon(TexInfo Model, Vec3 Position, Vec3 Scale, s16 Rotation, Vec3 Translation,
                 Vec2 TopRight, Vec2 TopLeft, Vec2 BottomLeft, Vec2 BottomRight, int Opacity, int BottomSkew)
    {
        if (Opacity == 0) return;

        NNS_G3dGeFlushBuffer();
        G3_LoadMtx43(0x02085B20);

        REG_MTX_TRANSLATE = Position.x;
        REG_MTX_TRANSLATE = Position.y;
        REG_MTX_TRANSLATE = Position.z;

        if (Rotation != 0)
        {
            glTranslatef32(Translation.x,Translation.y,Translation.z);
            glRotateZi(Rotation);
            glTranslatef32(-Translation.x,-Translation.y,-Translation.z);
        }

        REG_MTX_SCALE = Scale.x;
        REG_MTX_SCALE = Scale.y;
        REG_MTX_SCALE = Scale.z;

        REG_MTX_CONTROL = 3;
        REG_MTX_IDENTITY = 0;
        REG_MTX_CONTROL = 2;

        REG_POLY_FORMAT = (0x3880 | (Opacity << 16));
        REG_TEXT_FORMAT = Model.texparam;
        REG_PAL_BASE = Model.palbase;

        REG_COLOR= 0x7FFF;

        REG_GFX_BEGIN = 1;
        texcoord2(TopRight.x, TopRight.y);
        vec3(0x1000,    0x1000,     0x0);

        texcoord2(TopLeft.x, TopLeft.y);
        vec3(0x0000,    0x1000,     0x0);

        texcoord2(BottomLeft.x, BottomLeft.y);
        vec3(0x0000 + BottomSkew,    0x0000,     0x0);

        texcoord2(BottomRight.x, BottomRight.y);
        vec3(0x1000 + BottomSkew,    0x0000,     0x0);

        REG_GFX_END = 0;
    }