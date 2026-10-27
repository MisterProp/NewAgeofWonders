

int * AoWE_TStructure_Destroy_23EDC2EF(int *param_1,uint param_2)



{

  int *piVar1;

  

                    /* 0x5e5c8  2310  AoWE.TStructure.Destroy@23EDC2EF */

  if (param_1[10] != 0) {

    (**(code **)(*param_1 + 0x194))();

  }

  piVar1 = (int *)param_1[1];

  if (((piVar1 != (int *)0x0) && ((*(byte *)(param_1 + 3) & 2) != 0)) &&

     (*(char *)(param_1[2] + 0x44) != '\0')) {

    (**(code **)(*piVar1 + 0x74))(piVar1,0x20106);

  }

  HSEngine_THexagonSprite_Destroy_23EDC2EF(param_1,param_2 & 0xfffffffe);

  if ((AoWE_AoWHSMap_6B957A58 != 0) && ((*(byte *)(AoWE_AoWHSMap_6B957A58 + 0x3c) & 8) == 0)) {

    AoWE_TAoWHSMap_StructureDestroyed_23EDC2EF(AoWE_AoWHSMap_6B957A58,param_1);

  }

  if ((char)param_2 != '\0') {

    System__ClassDestroy_51F89FF7(param_1);

  }

  return param_1;

}




