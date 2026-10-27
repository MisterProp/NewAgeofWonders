// /AoWEPACK.dpl : 557bdbe0

undefined4
PrdPlace_TProductionPlaceControl_UnitProductionGold_23EDC2EF(int param_1,undefined4 param_2)

{
  char cVar1;
  undefined4 uVar2;
  
                    /* 0xbdbbc  7494  PrdPlace.TProductionPlaceControl.UnitProductionGold@23EDC2EF
                        */
  cVar1 = (**(code **)(**(int **)(param_1 + 0x28) + 0x230))();
  if (cVar1 != '\b') {
    return 0;
  }
  uVar2 = AoWE_TProductionControl_UnitProductionGold_23EDC2EF(param_1,param_2);
  return uVar2;
}

