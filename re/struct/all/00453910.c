
void FUN_00453910(int param_1)

{
  undefined *puVar1;
  code *pcVar2;
  
  pcVar2 = AoWE_AoWEngine_7E705837_exref;
  puVar1 = PTR_DAT_0045a158;
  Sound_ReleaseSoundSystem_51F89FF7();
  FUN_0045660c(*(undefined4 *)PTR_DAT_0045a420);
  AoWReg_TAoWRegistry_SetMainWindowState_23EDC2EF
            (*(undefined4 *)(*(int *)pcVar2 + 0x5c),*(undefined1 *)(param_1 + 0x136));
  AoWReg_TAoWRegistry_SetString_23EDC2EF
            (*(undefined4 *)(*(int *)pcVar2 + 0x5c),0,
             *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a598 + 0x90) + 0x90));
  AoWReg_TAoWRegistry_SetString_23EDC2EF
            (*(undefined4 *)(*(int *)pcVar2 + 0x5c),1,
             *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a598 + 0x94) + 0x90));
  AoWReg_TAoWRegistry_SetDisplayWidth_23EDC2EF
            (*(undefined4 *)(*(int *)pcVar2 + 0x5c),*(undefined4 *)(*(int *)PTR_DAT_0045a478 + 0x9c)
            );
  AoWReg_TAoWRegistry_SetDisplayHeight_23EDC2EF
            (*(undefined4 *)(*(int *)pcVar2 + 0x5c),*(undefined4 *)(*(int *)PTR_DAT_0045a478 + 0xa0)
            );
  if (*(int *)(*(int *)(param_1 + 0x1dc) + 0x1a8) == *(int *)(param_1 + 0x1e4)) {
    AoWReg_TAoWRegistry_SetDisplayMode_23EDC2EF(*(undefined4 *)(*(int *)pcVar2 + 0x5c),1);
  }
  else {
    AoWReg_TAoWRegistry_SetDisplayMode_23EDC2EF(*(undefined4 *)(*(int *)pcVar2 + 0x5c),0);
  }
  AoWReg_TAoWRegistry_SetUseDirectSound_23EDC2EF
            (*(undefined4 *)(*(int *)pcVar2 + 0x5c),*(undefined1 *)(*(int *)puVar1 + 0x11d));
  AoWReg_TAoWRegistry_SetSFXEnabled_23EDC2EF
            (*(undefined4 *)(*(int *)pcVar2 + 0x5c),*(undefined1 *)(*(int *)puVar1 + 0x124));
  AoWReg_TAoWRegistry_SetMusicEnabled_23EDC2EF
            (*(undefined4 *)(*(int *)pcVar2 + 0x5c),*(undefined1 *)(*(int *)puVar1 + 0x11c));
  AoWReg_TAoWRegistry_SetSoundQuality_23EDC2EF
            (*(undefined4 *)(*(int *)pcVar2 + 0x5c),*(undefined4 *)(*(int *)puVar1 + 0x110));
  AoWReg_TAoWRegistry_SetSFXVolume_23EDC2EF
            (*(undefined4 *)(*(int *)pcVar2 + 0x5c),
             *(undefined4 *)(*(int *)(*(int *)puVar1 + 0xec) + 0x148));
  AoWReg_TAoWRegistry_SetMusicVolume_23EDC2EF
            (*(undefined4 *)(*(int *)pcVar2 + 0x5c),
             *(undefined4 *)(*(int *)(*(int *)puVar1 + 0xe8) + 0x148));
  return;
}

