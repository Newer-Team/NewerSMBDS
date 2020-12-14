typedef struct
{
	u32 Header;
	bool Inited;
	u8 World;
	u8 Lives;
	u8 Coins;
	u8 StarCoins;
	u8 CompletionBitmask;
	u32 Score;
} SaveFileInfo;

typedef struct
{
	bool FSELheld;
	bool FSELInitDone;
	int FSELSelection;
	int currentScreen;
	int currentSlot;
	int copyslot;
	int Timer;
	int Booped;
	int currentmessage;
	int yesnoselection;
	int FadeTimer;
	u8* ENPGPointers[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	SaveFileInfo* SaveFileInfos[3] = {0, 0, 0};
} FSEL;

void InitFSEL();
void ExecuteFSEL();
void FSELCleanup();
void FSELGoBack();
void SetSaveInfo(int slot, SaveFileInfo* Info);
void ArrangeAFileScreen(int slot);
void ArrangeCopyScreen(int slot);
void FSELPlusMinus(int mode);
void MainScreenCode();
void EraseScreenCode();
void CopyScreenCode();
void ArrangeEraseScreen(int slot);
void ScrollTopScreenBottomLayer();
void ScrollOffLayer(int layer, int direction);
void ScrollInLayer(int layer, int direction);
void FooterDraw(bool hide, int color);
void FooterMove(int mode);
void DisplayMessage(int messageid, bool pressed);
void ChangeMessage(int newmessage, bool pressed);
void DrawYesNoButton(int button, int layer);
void ShowYesNoButtons(bool color);
void SwitchYesNoSelection(bool color);
void OperateOnYesNoButtons(bool color);
void DrawBottomScreenSlot(int slot, int layer, int style);
void ArrangeBottomScreen(int highlight);
void TouchedSlotInCopy(int slot);
void ConfirmInCopy();
void ArrangeBottomCopyScreen(int copied, int copiedto);
void MoveSlotInCopy(int mode);