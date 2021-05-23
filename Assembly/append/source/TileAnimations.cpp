#include "game.h" 

int frame = 0; 
int speedlimiter = 0;
u16* animationFile = NULL; 
int *tileset1ID = (int*)(0x020CACBC); 
int *worldID = (int*)(0x02088BFC);
bool *GameFrozen = (bool*)(0x020CA850);
extern bool IsSpecialLevel;


void copy8Tile(int dn, int sn) 
{ 
    u16* charPtr = (u16*)G2_GetBG2CharPtr(); 
    u16* destPtr = charPtr + dn * (8 * 8 / 2) + (256 * 48 / 2); 
    u16* srcPtr = animationFile + sn * (8 * 8 / 2);

    for(int i = 0; i < 8*8/2; i++) destPtr[i] = srcPtr[i]; 
} 


void copyTile(int dx, int dy, int sx, int sy) 
{ 
    sy += sx / 4;
    sx %= 4;
    copy8Tile(dx*2 + dy*64, sx*2+sy*16);
    copy8Tile(dx*2+1 + dy*64, sx*2+1+sy*16);
    copy8Tile(dx*2 + dy*64+32, sx*2+sy*16+8);
    copy8Tile(dx*2+1 + dy*64+32, sx*2+1+sy*16+8);
} 

//Stars
void hook_020b8370() 
{ 
    if (IsSpecialLevel) 
        return;
    
    if (*tileset1ID == 18 && *GameFrozen == 0) 
    {
        frame++; 
        if(frame == 4*6) frame = 0;
        copyTile(0, 6, frame%4, 0); 
        copyTile(9, 1, frame%4, 1); 
        copyTile(9, 2, frame%4, 3); 
        copyTile(1, 6, frame%4, 2); 
        copyTile(9, 3, frame%4, 4); 
    }
} 
    

//Munchers
void hook_020B68CC() 
{ 
    if (IsSpecialLevel) 
        return;
    
    if (*GameFrozen) 
        return;

    if (*tileset1ID == 31) 
    {
        if (speedlimiter==4) frame++;
        if (speedlimiter==5) speedlimiter=0;
        if(frame == 4*6) frame = 0;
        copyTile(9, 4, frame%4, 0); 
        copyTile(13, 4, frame%4, 1); 
        copyTile(10, 4, frame%4, 2); 
        copyTile(14, 4, frame%4, 3); 
        copyTile(11, 4, frame%4, 4); 
        copyTile(15, 4, frame%4, 5); 
        copyTile(12, 4, frame%4, 6); 
        copyTile(13, 5, frame%4, 7); 
        speedlimiter++;
    }
    if (*tileset1ID == 4)  
    {
        if (speedlimiter==3) frame++;
        if (speedlimiter==4) speedlimiter=0;
        if (frame == 4*6) frame=0;
        copyTile(15, 0, frame%4, 0); 
        copyTile(14, 2, frame%4, 1); 
        copyTile(15, 5, frame%4, 2); 
        copyTile(10, 2, frame%4, 3); 
        copyTile(7, 6, frame%4, 4); 
        copyTile(0, 6, frame%4, 5); 
        speedlimiter++;
    }
    if (*tileset1ID == 16)  
    {
        if (speedlimiter==3) frame++;
        if (speedlimiter==4) speedlimiter=0;
        if (frame == 4*6) frame=0;
        copyTile(15, 2, frame%4, 0); 
        copyTile(14, 4, frame%4, 1); 
        copyTile(15, 4, frame%4, 2); 
        copyTile(14, 5, frame%4, 3); 
        copyTile(15, 5, frame%4, 4); 
        copyTile(15, 3, frame%4, 5); 
        copyTile(12, 5, frame%4, 6); 
        copyTile(12, 6, frame%4, 7); 
        speedlimiter++;
    }
    if (*tileset1ID == 7)  
    {
        if (speedlimiter==2) frame++;
        if (speedlimiter==3) speedlimiter=0;
        if (frame == 4*6) frame=0;
        copyTile(0, 0, frame%4, 0); 
        copyTile(1, 0, frame%4, 1); 
        copyTile(2, 0, frame%4, 2); 
        copyTile(0, 2, frame%4, 3); 
        copyTile(0, 1, frame%4, 4); 
        copyTile(1, 1, frame%4, 5); 
        copyTile(2, 1, frame%4, 6); 
        copyTile(1, 2, frame%4, 7); 
    //  copyTile(5, 3, frame%4, 8); 
        speedlimiter++;
    }
} 

void hook_020BBB3C() 
{ 
    if (IsSpecialLevel) 
        return;
    
    int size = 0;
    int fileId = 0;
    
    switch(*tileset1ID)
    {
        case 4:
        {
            size = 6144; 
            fileId = 164 - 0x83;    //snowball.narc     
            break;          
        }
        case 7:
        {
            size = 9216; 
            fileId = 2630 - 0x83;   
            break;              
        }
        case 16:
        {
            size = 8192; 
            fileId = 2627 - 0x83;
            break;              
        }
        case 18:
        {
            size = 5120;    
            fileId = 152 - 0x83;    //Menu_title.narc
            break;
        }
        case 31:
        {
            size = 8192; 
            fileId = 2636 - 0x83;       
            break;              
        }
        default: break;
    }
            
    if (size)   
    {
        freeToCurrentHeapIfNotNull(animationFile);
        animationFile = (u16*) allocFromCurrentHeap(size, 0); 
        loadFileByExtId_LZ_Dest(fileId, animationFile); 
    }

}


void hook_020BB6A8_ov_00()
{
    freeToCurrentHeapIfNotNull(animationFile);
    animationFile = NULL;
    frame = 0;
    speedlimiter = 0;
}