// /AoWEPACK.dpl : 5575dad8

undefined4
AoWE_TProductionControl_RetryProductionExecution_23EDC2EF(int *param_1,undefined4 param_2)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  int *piVar4;
  int *piVar5;
  
                    /* 0x5dad8  2331  AoWE.TProductionControl.RetryProductionExecution@23EDC2EF */
  iVar2 = (**(code **)(*(int *)param_1[5] + 0x54))();
  if ((iVar2 != 0) && ((char)param_1[8] == '\x01')) {
    cVar1 = (**(code **)(*param_1 + 0x7c))();
    AoWE_TPlayerList_GetPlayers_23EDC2EF(*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),(int)cVar1)
    ;
    cVar1 = AoWE_TPlayer_GetBusy_23EDC2EF();
    if (cVar1 == '\0') {
      uVar3 = AoWE_TProductionQueue_GetItem_23EDC2EF(param_1[5],0);
      cVar1 = (**(code **)(*param_1 + 0xb0))(param_1,uVar3,param_2);
      if (cVar1 != '\0') {
        piVar4 = (int *)(**(code **)(*param_1 + 0x74))();
        piVar4[5] = 3;
        cVar1 = (**(code **)(*param_1 + 0x7c))();
        piVar5 = (int *)NetworkE_TTokenManager_GetTokenControl_23EDC2EF
                                  (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x23c),(int)cVar1);
        (**(code **)(*piVar5 + 0x10))(piVar5,piVar4);
        (**(code **)(*piVar4 + 0x2c))();
        return 1;
      }
      return 0;
    }
  }
  return 0;
}

