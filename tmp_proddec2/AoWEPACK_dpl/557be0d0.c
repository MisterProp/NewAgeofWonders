// /AoWEPACK.dpl : 557be0d0

void PrdPlace_TProductionPlaceControl_GetProductionNameProgressive_23EDC2EF
               (int param_1,int param_2,undefined4 param_3)

{
                    /* 0xbe0d0  7481
                       PrdPlace.TProductionPlaceControl.GetProductionNameProgressive@23EDC2EF */
  if ((*(int *)(param_2 + 8) == 1) || (*(int *)(param_2 + 8) == 2)) {
    AoWE_TProductionControl_GetProductionNameProgressive_23EDC2EF(param_1,param_2,param_3);
  }
  else {
    (**(code **)(**(int **)(param_1 + 0x28) + 0x238))(*(int **)(param_1 + 0x28),param_2,param_3);
  }
  return;
}

