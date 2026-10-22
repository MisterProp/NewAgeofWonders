
void Farm_TFarm_UpdateCrops_23EDC2EF(int *param_1)

{
  char cVar1;
  int iVar2;
  
                    /* 0xb2cd0  7105  Farm.TFarm.UpdateCrops@23EDC2EF */
  cVar1 = (**(code **)(*param_1 + 0x84))();
  if ((cVar1 == '\0') && ((*(byte *)(param_1 + 3) & 0x20) != 0)) {
    cVar1 = (**(code **)(*param_1 + 0x150))();
    if ((cVar1 == '\0') && ((char)param_1[6] != '\x05')) {
      if ((*(byte *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x3c) & 2) == 0) {
        iVar2 = -1;
      }
      else {
        iVar2 = *(int *)(*(int *)(param_1[2] + 0x74) + 8);
      }
    }
    else {
      iVar2 = 0;
    }
    if (iVar2 != -1) {
      Crops_TPlayerCropStructure_SetCropCount_23EDC2EF(param_1,iVar2);
    }
    Crops_TPlayerCropStructure_UpdateCropSettings_23EDC2EF(param_1);
    (**(code **)(*param_1 + 0x230))();
  }
  return;
}

