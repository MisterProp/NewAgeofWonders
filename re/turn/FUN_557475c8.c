
undefined4 FUN_557475c8(char param_1)

{
  char cVar1;
  int iVar2;
  int *piVar3;
  int *piVar4;
  int iVar5;
  
  iVar5 = (int)param_1;
  AoWE_TPlayerList_GetPlayers_23EDC2EF(*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),iVar5);
  cVar1 = AoWE_TPlayer_GetBusy_23EDC2EF();
  if ((cVar1 == '\0') &&
     (iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),iVar5),
     *(char *)(iVar2 + 0x2c) != '\0')) {
    piVar3 = (int *)NetworkE_TTokenEvent_Create_23EDC2EF(&AoWE__TEndTurnTE_3647BA05,1,0);
    *(char *)(piVar3 + 4) = param_1;
    piVar3[5] = *(int *)(AoWE_AoWHSMap_6B957A58 + 0x174);
    *(undefined1 *)(piVar3 + 6) = 1;
    piVar4 = (int *)NetworkE_TTokenManager_GetTokenControl_23EDC2EF
                              (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x23c),iVar5);
    (**(code **)(*piVar4 + 0x10))(piVar4,piVar3);
    (**(code **)(*piVar3 + 0x2c))();
    return 1;
  }
  return 0;
}

