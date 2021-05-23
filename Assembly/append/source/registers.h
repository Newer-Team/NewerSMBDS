/*3D graphics stuff*/
#define REG_GFX_FIFO        (*(vu32*) 0x4000400)
#define REG_GFX_STATUS      (*(vu32*) 0x4000600)
#define REG_GFX_CONTROL     (*(vu16*) 0x4000060)
#define REG_COLOR           (*(vu32*) 0x4000480)
#define REG_VERTEX16        (*(vu32*) 0x400048C)
#define REG_TEXT_COORD      (*(vu32*) 0x4000488)
#define REG_TEXT_FORMAT     (*(vu32*) 0x40004A8)

#define REG_PAL_BASE        (*(vu32*) 0x40004AC)

#define REG_CLEAR_COLOR     (*(vu32*) 0x4000350)
#define REG_CLEAR_DEPTH     (*(vu16*) 0x4000354)

#define REG_LIGHT_VECTOR    (*(vu32*) 0x40004C8)
#define REG_LIGHT_COLOR     (*(vu32*) 0x40004CC)
#define REG_NORMAL          (*(vu32*) 0x4000484)

#define REG_DIFFUSE_AMBIENT     (*(vu32*) 0x40004C0)
#define REG_SPECULAR_EMISSION   (*(vu32*) 0x40004C4)
#define REG_SHININESS           (*(vu32*) 0x40004D0)

#define REG_POLY_FORMAT     (*(vu32*) 0x40004A4)

#define REG_GFX_BEGIN       (*(vu32*) 0x4000500)
#define REG_GFX_END         (*(vu32*) 0x4000504)
#define REG_GFX_FLUSH       (*(vu32*) 0x4000540)
#define REG_GFX_VIEWPORT    (*(vu32*) 0x4000580)

#define REG_MTX_CONTROL     (*(vu32*) 0x4000440)
#define REG_MTX_PUSH        (*(vu32*) 0x4000444)
#define REG_MTX_POP         (*(vu32*) 0x4000448)
#define REG_MTX_SCALE       (*(vint32*) 0x400046C)
#define REG_MTX_TRANSLATE   (*(vint32*) 0x4000470)
#define REG_MTX_RESTORE     (*(vu32*) 0x4000450)
#define REG_MTX_STORE       (*(vu32*) 0x400044C)
#define REG_MTX_IDENTITY    (*(vu32*) 0x4000454)
#define REG_MTX_LOAD4x4     (*(volatile f32*) 0x4000458)
#define REG_MTX_LOAD4x3     (*(volatile f32*) 0x400045C)
#define REG_MTX_MULT4x4     (*(volatile f32*) 0x4000460)
#define REG_MTX_MULT4x3     (*(volatile f32*) 0x4000464)
#define REG_MTX_MULT3x3     (*(volatile f32*) 0x4000468)