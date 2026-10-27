// /AoWEPACK.dpl : 5575dc7c

void AoWE_TProductionControl_ProductionDone_23EDC2EF(int *param_1)

{
  int *piVar1;
  char cVar2;
  undefined4 uVar3;
  int iVar4;
  
                    /* 0x5dc7c  2327  AoWE.TProductionControl.ProductionDone@23EDC2EF */
  AoWE_TProductionControl_BeginUpdate_23EDC2EF(param_1);
  if ((char)param_1[6] != '\0') {
    uVar3 = AoWE_TProductionQueue_GetItem_23EDC2EF(param_1[5],0);
    cVar2 = (**(code **)(*param_1 + 0x88))(param_1,uVar3);
    if (cVar2 != '\0') {
      piVar1 = (int *)param_1[5];
      iVar4 = (**(code **)(*piVar1 + 0x54))();
      Classes_TList_Move_23EDC2EF(piVar1[2],0,iVar4 + -1);
      goto LAB_5575dccf;
    }
  }
  AoWE_TProductionQueue_GetItem_23EDC2EF(param_1[5],0);
  System_TObject_Free_23EDC2EF();
LAB_5575dccf:
  iVar4 = (**(code **)(*(int *)param_1[5] + 0x54))();
  if (iVar4 == 0) {
    *(undefined1 *)(param_1 + 8) = 0;
  }
  else {
    uVar3 = AoWE_TProductionQueue_GetItem_23EDC2EF(param_1[5],0);
    (**(code **)(*param_1 + 0x60))(param_1,uVar3);
  }
  AoWE_TProductionControl_EndUpdate_23EDC2EF(param_1);
  return;
}

