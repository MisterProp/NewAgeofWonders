// /AoWEPACK.dpl : 5575d490

undefined4 AoWE_TProductionControl_ExecuteProductionTE_23EDC2EF(int *param_1,int param_2)

{
  int iVar1;
  undefined4 uVar2;
  
                    /* 0x5d490  2352  AoWE.TProductionControl.ExecuteProductionTE@23EDC2EF */
  switch(*(undefined4 *)(param_2 + 0x14)) {
  case 0:
    AoWE_TProductionControl_BeginUpdate_23EDC2EF(param_1);
    if (*(char *)(param_2 + 0x1c) == '\0') {
      (**(code **)(*param_1 + 0x6c))();
    }
    (**(code **)(*param_1 + 100))(param_1,*(undefined4 *)(param_2 + 0x18));
    AoWE_TProductionControl_EndUpdate_23EDC2EF(param_1);
    break;
  case 1:
    (**(code **)(*param_1 + 0x68))(param_1,*(undefined4 *)(param_2 + 0x20));
    break;
  case 2:
    (**(code **)(*param_1 + 0x6c))();
    break;
  case 3:
    iVar1 = (**(code **)(*(int *)param_1[5] + 0x54))();
    if ((iVar1 != 0) && ((char)param_1[8] == '\x01')) {
      uVar2 = AoWE_TProductionQueue_GetItem_23EDC2EF(param_1[5],0);
      (**(code **)(*param_1 + 0x60))(param_1,uVar2);
    }
    break;
  case 4:
    *(bool *)(param_1 + 6) = *(int *)(param_2 + 0x20) == 1;
    (**(code **)(*param_1 + 0x98))();
  }
  return 1;
}

