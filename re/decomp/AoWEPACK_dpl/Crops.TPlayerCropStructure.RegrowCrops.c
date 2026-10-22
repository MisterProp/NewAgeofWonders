
void Crops_TPlayerCropStructure_RegrowCrops_23EDC2EF(int param_1)

{
  int iVar1;
  
                    /* 0xa61b4  6740  Crops.TPlayerCropStructure.RegrowCrops@23EDC2EF */
  iVar1 = Crops_TPlayerCropStructure_GetCropCount_23EDC2EF(param_1);
  if (iVar1 < *(int *)(*(int *)(*(int *)(param_1 + 8) + 0x74) + 8)) {
    iVar1 = Crops_TPlayerCropStructure_GetCropCount_23EDC2EF(param_1);
    Crops_TPlayerCropStructure_SetCropCount_23EDC2EF(param_1,iVar1 + 1);
  }
  return;
}

