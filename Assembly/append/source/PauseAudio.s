
@ 0x7F is 100%
#define WORLD_MAP_PAUSE_MUSIC_VOLUME #0x2A


repl_020CED60_ov_08: @ Replacing pauseMusic upon opening the pause menu on world maps
    STMFD   SP!, {R0-R12,LR}
    LDR     R0, =#0x208FC08
    LDR     R1, =#0xFFFF
    MOV     R2, WORLD_MAP_PAUSE_MUSIC_VOLUME
    BLX     NNS_SndPlayerSetTrackVolume
    LDMFD   SP!, {R0-R12,LR}
    BX      LR


repl_020CE7E4_ov_08: @ Replacing pauseMusic upon closing the pause menu on world maps
    STMFD   SP!, {R0-R12,LR}
    LDR     R0, =#0x208FC08
    LDR     R1, =#0xFFFF
    MOV     R2, #0x7F
    BLX     NNS_SndPlayerSetTrackVolume
    LDMFD   SP!, {R0-R12,LR}
    BX      LR


repl_020CE780_ov_08: @ SFX for closing the pause menu on world maps
    MOV     R0, #0xEA
    BX      LR


repl_020A2518_ov_00: @ SFX for closing the pause menu in levels
    MOV     R0, #0xEA
    BX      LR
