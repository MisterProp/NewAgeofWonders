// /AoWEPACK.dpl : 557be000

undefined1
PrdPlace_TProductionPlaceControl_CanExecuteProduction_23EDC2EF(undefined4 param_1,int param_2)

{
  undefined1 uVar1;
  
                    /* 0xbe000  7485  PrdPlace.TProductionPlaceControl.CanExecuteProduction@23EDC2EF
                        */
  if (*(int *)(param_2 + 8) == 10) {
    return 1;
  }
  uVar1 = AoWE_TProductionControl_CanExecuteProduction_23EDC2EF();
  return uVar1;
}

