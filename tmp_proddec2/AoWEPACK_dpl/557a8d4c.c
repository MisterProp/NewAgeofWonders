// /AoWEPACK.dpl : 557a8d4c

void City_TCityProductionControl_Update_23EDC2EF(int param_1)

{
                    /* 0xa8d4c  6955  City.TCityProductionControl.Update@23EDC2EF */
  AoWE_TProductionControl_Update_23EDC2EF(param_1);
  if (*(int *)(param_1 + 0x10) == 0) {
    (**(code **)(**(int **)(param_1 + 0x28) + 0x1c8))();
  }
  return;
}

