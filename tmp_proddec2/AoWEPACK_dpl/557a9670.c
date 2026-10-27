// /AoWEPACK.dpl : 557a9670

undefined4 City_TCity_CanDblClick_23EDC2EF(int *param_1)

{
  char cVar1;
  
                    /* 0xa9670  6940  City.TCity.CanDblClick@23EDC2EF */
  if (*(char *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0xa5) == (char)param_1[0xc]) {
    cVar1 = (**(code **)(*param_1 + 0x150))();
    if (cVar1 == '\0') {
      cVar1 = AoWE_TStructure_GetBuilding_23EDC2EF(param_1);
      if (cVar1 == '\0') {
        return 1;
      }
    }
  }
  return 0;
}

