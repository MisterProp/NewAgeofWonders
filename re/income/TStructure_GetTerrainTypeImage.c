

undefined4 AoWE_TStructure_GetTerrainTypeImage_23EDC2EF(int *param_1,int param_2)



{

  char cVar1;

  undefined4 uVar2;

  

                    /* 0x5e6a0  2304  AoWE.TStructure.GetTerrainTypeImage@23EDC2EF */

  cVar1 = (**(code **)(*param_1 + 0x150))();

  if (cVar1 == '\0') {

    cVar1 = AoWE_TStructure_GetBuilding_23EDC2EF(param_1);

    if (cVar1 == '\0') {

      uVar2 = ILTer_TILTerrainMO_GetTerrainTypeImage_23EDC2EF(param_1,param_2);

    }

    else if (param_2 == 3) {

      uVar2 = 0x1a;

    }

    else if (param_2 == 5) {

      uVar2 = 0x1b;

    }

    else {

      uVar2 = 0x19;

    }

  }

  else if (param_2 == 3) {

    uVar2 = 0x17;

  }

  else {

    if (param_2 != 5) {

      return 0x16;

    }

    uVar2 = 0x18;

  }

  return uVar2;

}




