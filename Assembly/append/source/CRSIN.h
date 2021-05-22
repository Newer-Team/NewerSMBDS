typedef struct
{
    bool CRSINHeld;
    bool CRSINInitDone;
    bool ScrollingIn;
    bool StateJustChanged;
    bool DoingBannerDrop;
    bool FirstTimeInWaitState;
    bool SelectBlinkIsBig;
    bool SelectionArrowPressed;

    int Timer;
    int FadeInTimer;
    int BannerDropTimer;
    int SelectBlinkTimer;
    int State;

    u16 topScreenImg;
    u16 subScreenImg;
	
	int SecretPressedSelects;
	
}   CRSIN;

void InitCRSIN();
void CRSIN_LoadImages();
void CRSIN_SetupImages();
void CRSIN_SetupLives();
void CRSIN_dtor();
void CRSIN_Scroll();
void CRSIN_Wait();
void CRSIN_DoPlayerSelect();
void CRSIN_Exit(void* NSMB_CRSIN);
void CRSIN_FadeInWait();
