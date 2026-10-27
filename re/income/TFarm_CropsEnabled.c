

undefined4 Farm_TFarm_CropsEnabled_23EDC2EF(int param_1)



{

  char cVar1;

  

                    /* 0xb2c88  7107  Farm.TFarm.CropsEnabled@23EDC2EF */

  cVar1 = Crops_TPlayerCropStructure_CropsEnabled_23EDC2EF(param_1);

  if ((cVar1 != '\0') && (*(char *)(param_1 + 0x18) != '\x05')) {

    return CONCAT31(*(char *)(param_1 + 0x18) >> 7,1);

  }

  return 0;

}




