
undefined4 AoWE_TProductionControl_SetLoopQueueTo_23EDC2EF(int *param_1,char param_2)

{
  char cVar1;
  int *piVar2;
  int *piVar3;
  
                    /* 0x5db80  2330  AoWE.TProductionControl.SetLoopQueueTo@23EDC2EF */
  if (param_2 != (char)param_1[6]) {
    cVar1 = (**(code **)(*param_1 + 0x7c))();
    AoWE_TPlayerList_GetPlayers_23EDC2EF(*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),(int)cVar1)
    ;
    cVar1 = AoWE_TPlayer_GetBusy_23EDC2EF();
    if (cVar1 == '\0') {
      piVar2 = (int *)(**(code **)(*param_1 + 0x74))();
      piVar2[5] = 4;
      if (param_2 == '\0') {
        piVar2[8] = 0;
      }
      else {
        piVar2[8] = 1;
      }
      cVar1 = (**(code **)(*param_1 + 0x7c))();
      piVar3 = (int *)NetworkE_TTokenManager_GetTokenControl_23EDC2EF
                                (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x23c),(int)cVar1);
      (**(code **)(*piVar3 + 0x10))(piVar3,piVar2);
      (**(code **)(*piVar2 + 0x2c))();
      return 1;
    }
  }
  return 0;
}

