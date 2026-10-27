// /AoWEPACK.dpl : 5575d324

void AoWE_TProductionControl_ExecuteAddProduction_23EDC2EF(int *param_1,undefined4 param_2)

{
  int *piVar1;
  int iVar2;
  
                    /* 0x5d324  2357  AoWE.TProductionControl.ExecuteAddProduction@23EDC2EF */
  AoWE_TProductionControl_BeginUpdate_23EDC2EF(param_1);
  piVar1 = (int *)Engine_TEObject_Copy_23EDC2EF(param_2,AoWE_AoWEngine_7E705837);
  (**(code **)(*piVar1 + 8))(piVar1,param_1[5]);
  iVar2 = (**(code **)(*(int *)param_1[5] + 0x54))();
  if (iVar2 == 1) {
    (**(code **)(*param_1 + 0x60))(param_1,piVar1);
  }
  AoWE_TProductionControl_EndUpdate_23EDC2EF(param_1);
  return;
}

