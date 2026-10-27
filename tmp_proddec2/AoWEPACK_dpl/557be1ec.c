// /AoWEPACK.dpl : 557be1ec

undefined4 PrdPlace_TProductionPlace_CanDblClick_23EDC2EF(int *param_1)

{
  char cVar1;
  
                    /* 0xbe1ec  7477  PrdPlace.TProductionPlace.CanDblClick@23EDC2EF */
  if (*(char *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0xa5) == (char)param_1[0xc]) {
    cVar1 = (**(code **)(*param_1 + 0x230))();
    if (cVar1 == '\b') {
      return 1;
    }
  }
  return 0;
}

