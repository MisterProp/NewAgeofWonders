
void AoWE_TProductionControl_GetQueueLocked_23EDC2EF(int *param_1)

{
  char cVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  
                    /* 0x5cd20  2365  AoWE.TProductionControl.GetQueueLocked@23EDC2EF */
  iVar2 = (**(code **)(*(int *)param_1[5] + 0x54))();
  iVar3 = (**(code **)(*param_1 + 0x80))();
  cVar1 = iVar3 <= iVar2;
  while ((cVar1 == '\0' && (iVar2 != 0))) {
    iVar2 = iVar2 + -1;
    uVar4 = AoWE_TProductionQueue_GetItem_23EDC2EF(param_1[5],iVar2);
    cVar1 = (**(code **)(*param_1 + 0x8c))(param_1,uVar4);
  }
  return;
}

