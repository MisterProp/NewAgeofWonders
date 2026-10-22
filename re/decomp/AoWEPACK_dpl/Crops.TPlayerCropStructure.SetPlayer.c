
void Crops_TPlayerCropStructure_SetPlayer_23EDC2EF(int *param_1,undefined4 param_2)

{
  undefined4 uVar1;
  
                    /* 0xa6278  6735  Crops.TPlayerCropStructure.SetPlayer@23EDC2EF */
  if ((char)param_2 != (char)param_1[0xc]) {
    if ((char)param_1[0xc] != -1) {
      uVar1 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),
                         (int)(char)param_1[0xc]);
      AoWE_TPlayer_UnregisterIncomeSource_23EDC2EF(uVar1,param_1[0x10]);
    }
    AoWE_TPlayerStructure_SetPlayer_23EDC2EF(param_1,param_2);
    (**(code **)(*param_1 + 0x234))();
    if ((char)param_1[0xc] != -1) {
      uVar1 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),
                         (int)(char)param_1[0xc]);
      AoWE_TPlayer_RegisterIncomeSource_23EDC2EF(uVar1,param_1[0x10]);
    }
  }
  return;
}

