// /AoWEPACK.dpl : 557bdfe0

void PrdPlace_TProductionPlaceControl_Update_23EDC2EF(int param_1)

{
                    /* 0xbdfe0  7486  PrdPlace.TProductionPlaceControl.Update@23EDC2EF */
  AoWE_TProductionControl_Update_23EDC2EF(param_1);
  if (*(int *)(param_1 + 0x10) == 0) {
    (**(code **)(**(int **)(param_1 + 0x28) + 0x1c8))();
  }
  return;
}

