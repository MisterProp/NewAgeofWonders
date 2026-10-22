
void FUN_004521f4(int param_1)

{
  int iVar1;
  
  if (*(int *)(param_1 + 0x4c) != 0) {
    (**(code **)(**(int **)(param_1 + 0x4c) + 0x48))();
    System__IntfClear_E37E18EC(param_1 + 0x4c);
  }
  if (*(int *)(param_1 + 0x54) != 0) {
    System__IntfClear_E37E18EC(param_1 + 0x54);
  }
  iVar1 = AoWReg_TAoWRegistry_GetSoundQuality_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x5c));
  if (iVar1 != 0) {
    Sound_InitializeSoundSystem_8AEF5D00("Galaxy",*(int *)PTR_DAT_0045a158 + 0x114);
    AoWReg_TAoWRegistry_GetMusicVolume_23EDC2EF
              (*(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x5c));
    Sound_SetMusicVolume_A03DAC8A();
    AoWReg_TAoWRegistry_GetSFXVolume_23EDC2EF
              (*(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x5c));
    Sound_SetSFXVolume_A03DAC8A();
  }
  return;
}

