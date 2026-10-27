// /AoWEPACK.dpl : 557be010

void PrdPlace_TProductionPlaceControl_NewTurn_23EDC2EF(int param_1,undefined4 param_2)

{
  int iVar1;
  
                    /* 0xbe010  7484  PrdPlace.TProductionPlaceControl.NewTurn@23EDC2EF */
  AoWE_TProductionControl_NewTurn_23EDC2EF(param_1,param_2);
  if (((*(char *)(param_1 + 0x20) == '\x02') &&
      ((char)param_2 == *(char *)(*(int *)(param_1 + 0x24) + 0x1d))) &&
     (*(char *)(*(int *)(param_1 + 0x24) + 0x10) == '\0')) {
    iVar1 = AoWE_TProductionQueue_GetItem_23EDC2EF(*(undefined4 *)(param_1 + 0x14),0);
    if (*(int *)(iVar1 + 8) == 10) {
      AoWE_TProductionControl_ProductionDone_23EDC2EF(param_1);
    }
  }
  return;
}

