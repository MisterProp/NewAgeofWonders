
undefined1 Crops_TPlayerCropStructure_GetRace_23EDC2EF(int param_1)

{
  int iVar1;
  
                    /* 0xa6450  6730  Crops.TPlayerCropStructure.GetRace@23EDC2EF */
  if ('\0' < *(char *)(param_1 + 0x30)) {
    iVar1 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),
                       (int)*(char *)(param_1 + 0x30));
    return *(undefined1 *)(iVar1 + 0xa5);
  }
  return 0xff;
}

