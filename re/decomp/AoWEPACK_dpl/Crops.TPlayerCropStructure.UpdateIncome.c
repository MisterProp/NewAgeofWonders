
void Crops_TPlayerCropStructure_UpdateIncome_23EDC2EF(int *param_1)

{
  char cVar1;
  int iVar2;
  
                    /* 0xa6230  6737  Crops.TPlayerCropStructure.UpdateIncome@23EDC2EF */
  cVar1 = (**(code **)(*param_1 + 0x150))();
  if (cVar1 == '\0') {
    cVar1 = AoWE_TStructure_GetBuilding_23EDC2EF(param_1);
    if (cVar1 == '\0') {
      iVar2 = Crops_TPlayerCropStructure_GetCropCount_23EDC2EF(param_1);
      Crops_TPlayerCropStructure_SetIncome_23EDC2EF(param_1,iVar2 * 2);
      return;
    }
  }
  Crops_TPlayerCropStructure_SetIncome_23EDC2EF(param_1,0);
  return;
}

