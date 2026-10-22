
void Crops_TPlayerCropStructure_UpdateCrops_23EDC2EF(int *param_1)

{
  char cVar1;
  
                    /* 0xa63f8  6731  Crops.TPlayerCropStructure.UpdateCrops@23EDC2EF */
  cVar1 = (**(code **)(*param_1 + 0x84))();
  if ((cVar1 == '\0') && ((*(byte *)(param_1 + 3) & 0x20) != 0)) {
    if ((*(byte *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x3c) & 2) != 0) {
      cVar1 = (**(code **)(*param_1 + 0x228))();
      if (cVar1 != '\0') {
        Crops_TPlayerCropStructure_SetCropCount_23EDC2EF
                  (param_1,*(undefined4 *)(*(int *)(param_1[2] + 0x74) + 8));
      }
    }
    Crops_TPlayerCropStructure_UpdateCropSettings_23EDC2EF(param_1);
    (**(code **)(*param_1 + 0x230))();
  }
  return;
}

