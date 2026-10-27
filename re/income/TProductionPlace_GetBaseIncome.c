

undefined4 PrdPlace_TProductionPlace_GetBaseIncome_23EDC2EF(int param_1)



{

  char cVar1;

  

                    /* 0xbe4e0  7471  PrdPlace.TProductionPlace.GetBaseIncome@23EDC2EF */

  if (*(char *)(param_1 + 0x48) != '\x01') {

    cVar1 = AoWE_TStructure_GetBuilding_23EDC2EF(param_1);

    if (cVar1 == '\0') {

      return 10;

    }

  }

  return 0;

}




