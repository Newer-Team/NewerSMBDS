
void hook_02013238()
	{
        #ifdef GUNPEI
		if (*SceneParam == 0 && *SceneID == 4)							
            loadLevel(0xD, false, GUNPEI_WORLD, GUNPEI_LEVEL, GUNPEI_AREA, 0, 0, 0, 0, 1, GUNPEI_ENTRANCE, 1);
        #endif
    }    
    