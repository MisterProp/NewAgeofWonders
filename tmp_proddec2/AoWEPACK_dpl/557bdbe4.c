// /AoWEPACK.dpl : 557bdbe4

undefined4
PrdPlace_TProductionPlaceControl_UnitProductionMana_23EDC2EF(int param_1,undefined4 param_2)

{
  char cVar1;
  undefined4 uVar2;
  
                    /* 0xbdbe4  7493  PrdPlace.TProductionPlaceControl.UnitProductionMana@23EDC2EF
                        */
  cVar1 = (**(code **)(**(int **)(param_1 + 0x28) + 0x230))();
  if (cVar1 != '\b') {
    uVar2 = AoWE_TProductionControl_UnitProductionGold_23EDC2EF(param_1,param_2);
    return uVar2;
  }
  return 0;
}

