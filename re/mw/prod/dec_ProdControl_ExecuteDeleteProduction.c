
void AoWE_TProductionControl_ExecuteDeleteProduction_23EDC2EF(int *param_1,int param_2)

{
  int iVar1;
  undefined4 uVar2;
  
                    /* 0x5d368  2356  AoWE.TProductionControl.ExecuteDeleteProduction@23EDC2EF */
  AoWE_TProductionControl_BeginUpdate_23EDC2EF(param_1);
  if (param_2 == 0) {
    (**(code **)(*param_1 + 0x54))();
    AoWE_TProductionQueue_GetItem_23EDC2EF(param_1[5],0);
    System_TObject_Free_23EDC2EF();
    iVar1 = (**(code **)(*(int *)param_1[5] + 0x54))();
    if (iVar1 == 0) {
      *(undefined1 *)(param_1 + 8) = 0;
    }
    else {
      uVar2 = AoWE_TProductionQueue_GetItem_23EDC2EF(param_1[5],0);
      (**(code **)(*param_1 + 0x60))(param_1,uVar2);
    }
  }
  else {
    AoWE_TProductionQueue_GetItem_23EDC2EF(param_1[5],param_2);
    System_TObject_Free_23EDC2EF();
  }
  AoWE_TProductionControl_EndUpdate_23EDC2EF(param_1);
  return;
}

