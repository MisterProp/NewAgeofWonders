
undefined4
Crops_TPlayerCropStructure_PlaceHX_23EDC2EF
          (int *param_1,undefined4 param_2,undefined4 param_3,undefined1 param_4,undefined1 param_5)

{
  char cVar1;
  undefined4 uVar2;
  
                    /* 0xa6a14  6722  Crops.TPlayerCropStructure.PlaceHX@23EDC2EF */
  uVar2 = AoWE_TStructure_PlaceHX_23EDC2EF(param_1,param_2,param_3,param_4,param_5);
  if ((char)uVar2 != '\0') {
    cVar1 = (**(code **)(*param_1 + 0x84))();
    if (cVar1 == '\0') {
      (**(code **)(*param_1 + 0x234))();
    }
  }
  return uVar2;
}

