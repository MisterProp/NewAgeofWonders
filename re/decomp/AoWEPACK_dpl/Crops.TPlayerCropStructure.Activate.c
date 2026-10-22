
void Crops_TPlayerCropStructure_Activate_23EDC2EF(int *param_1)

{
  char cVar1;
  undefined4 uVar2;
  
                    /* 0xa62e4  6734  Crops.TPlayerCropStructure.Activate@23EDC2EF */
  AoWE_TPlayerStructure_Activate_23EDC2EF(param_1);
  if ((char)param_1[0xc] != -1) {
    uVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),
                       (int)(char)param_1[0xc]);
    AoWE_TPlayer_RegisterIncomeSource_23EDC2EF(uVar2,param_1[0x10]);
  }
  cVar1 = (**(code **)(*param_1 + 0x84))();
  if (cVar1 == '\0') {
    Crops_TPlayerCropStructure_LinkToCrops_23EDC2EF(param_1);
    (**(code **)(*param_1 + 0x234))();
  }
  return;
}

