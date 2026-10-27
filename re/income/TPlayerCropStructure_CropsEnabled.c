

undefined4 Crops_TPlayerCropStructure_CropsEnabled_23EDC2EF(int *param_1)



{

  char cVar1;

  

                    /* 0xa61dc  6739  Crops.TPlayerCropStructure.CropsEnabled@23EDC2EF */

  cVar1 = (**(code **)(*param_1 + 0x150))();

  if (cVar1 == '\0') {

    cVar1 = AoWE_TStructure_GetBuilding_23EDC2EF(param_1);

    if (cVar1 == '\0') {

      return 1;

    }

  }

  return 0;

}




