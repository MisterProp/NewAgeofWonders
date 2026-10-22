
undefined4 Farm_TFarm_CanPlaceCrop_23EDC2EF(undefined4 param_1,int param_2)

{
  char cVar1;
  
                    /* 0xb2ca8  7106  Farm.TFarm.CanPlaceCrop@23EDC2EF */
  if (*(char *)(param_2 + 0x14) != '\x05') {
    cVar1 = Crops_TPlayerCropStructure_CanPlaceCrop_23EDC2EF(param_1,param_2);
    if (cVar1 != '\0') {
      return 1;
    }
  }
  return 0;
}

