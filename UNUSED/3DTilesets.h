#ifndef _3DTILESETS_H
#define _3DTILESETS_H

typedef struct TileSettings
{
    u16 Index : 13;
    u16 TilesetNumber : 3;
    u8 Width;
    u8 Height;
    bool Disabled;
} TileSettings;

#endif