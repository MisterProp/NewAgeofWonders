
void AoWE_TTurnPlayerControl_EndTurn_23EDC2EF(int *param_1,undefined4 param_2,undefined4 param_3)

{
  char cVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  undefined4 uVar5;
  bool bVar6;
  
                    /* 0x56db8  2516  AoWE.TTurnPlayerControl.EndTurn@23EDC2EF */
  AoWE_TPlayerControl_EndTurn_23EDC2EF(param_1,param_2,param_3,param_3);
  cVar1 = (char)param_2;
  if (cVar1 == *(char *)(AoWE_AoWHSMap_6B957A58 + 0xa5)) {
    AoWE_TAoWHSMap_ZeroTurnTimer_23EDC2EF(AoWE_AoWHSMap_6B957A58);
    iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),(int)cVar1);
    if (*(char *)(iVar2 + 0x45) == '\x01') {
      AoWE_TAoWHSMap_ClearExecuteEventLogBuffer_23EDC2EF(AoWE_AoWHSMap_6B957A58);
    }
    HSEngine_THexagonSpriteSelection_UnselectLocation_23EDC2EF
              (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x18));
  }
  if (cVar1 == *(char *)(AoWE_AoWHSMap_6B957A58 + 0xa4)) {
    iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),(int)cVar1);
    if (*(char *)(iVar2 + 0xa7) != '\0') {
      AoWE_TAIExecuter_DeactivatePlayer_23EDC2EF
                (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x128),param_2);
    }
    piVar3 = (int *)EventLog_TEventLog_Create_23EDC2EF(&EventLog__TEndTurnEventLog_E299833A,1,0);
    iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),
                       (int)*(char *)(AoWE_AoWHSMap_6B957A58 + 0xa4));
    EventLog_TEventLogbook_AddEvent_23EDC2EF(*(undefined4 *)(iVar2 + 0xd8),piVar3);
    (**(code **)(*piVar3 + 0x2c))();
    iVar2 = Engine_TByteList_IndexOf_23EDC2EF
                      (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x170),
                       *(undefined1 *)(AoWE_AoWHSMap_6B957A58 + 0xa4));
    bVar6 = false;
    while (!bVar6) {
      iVar2 = iVar2 + 1;
      if (iVar2 == *(int *)(*(int *)(AoWE_AoWHSMap_6B957A58 + 0x170) + 8)) {
        (**(code **)(*param_1 + 0x50))();
        iVar2 = 0;
      }
      cVar1 = Engine_TByteList_Get_23EDC2EF(*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x170),iVar2);
      iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),(int)cVar1);
      bVar6 = *(char *)(iVar4 + 0x45) == '\x01';
      if (!bVar6) {
        AoWE_TPlayerControl_NewTurn_23EDC2EF(param_1,cVar1);
        AoWE_TPlayerControl_EndTurn_23EDC2EF(param_1,cVar1);
      }
    }
    if (*(int *)(*(int *)(AoWE_AoWHSMap_6B957A58 + 0x170) + 8) == 0) {
      (**(code **)(*param_1 + 0x54))(param_1,0);
    }
    else {
      uVar5 = Engine_TByteList_Get_23EDC2EF(*(int *)(AoWE_AoWHSMap_6B957A58 + 0x170),iVar2);
      (**(code **)(*param_1 + 0x54))(param_1,uVar5);
    }
  }
  return;
}

