
void AoWE_TSimultaniousPlayerControl_EndTurn_23EDC2EF(int *param_1,char param_2)

{
  bool bVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  int iVar6;
  undefined1 auStack_40 [24];
  int iStack_28;
  
                    /* 0x5911c  2484  AoWE.TSimultaniousPlayerControl.EndTurn@23EDC2EF */
  uVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x50],(int)param_2);
  AoWE_TPlayer_SetEndTurn_23EDC2EF(uVar2,1);
  bVar1 = true;
  iVar6 = 1;
  while (bVar1) {
    iVar3 = (**(code **)(*(int *)AoWE_AoWHSMap_6B957A58[0x50] + 0x54))();
    if (iVar6 == iVar3) break;
    iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x50],iVar6);
    if (*(char *)(iVar3 + 0x45) == '\x01') {
      iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x50],iVar6);
      if (*(char *)(iVar3 + 0x2c) != '\0') goto LAB_5575917f;
      bVar1 = false;
    }
    else {
LAB_5575917f:
      bVar1 = true;
    }
    iVar6 = iVar6 + 1;
  }
  if (bVar1) {
    if (*(char *)((int)AoWE_AoWHSMap_6B957A58 + 0xa5) == param_2) {
      AoWE_TAoWHSMap_ClearExecuteEventLogBuffer_23EDC2EF(AoWE_AoWHSMap_6B957A58);
    }
    AoWE_TAoWHSMap_LockRelationsChanged_23EDC2EF(AoWE_AoWHSMap_6B957A58);
    iVar6 = (**(code **)(*(int *)AoWE_AoWHSMap_6B957A58[0x50] + 0x54))();
    if (-1 < iVar6 + -1) {
      iVar3 = 0;
      do {
        iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x50],iVar3);
        if (*(char *)(iVar4 + 0x45) == '\x01') {
          iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x50],iVar3);
          if (*(char *)(iVar4 + 0xa7) != '\0') {
            AoWE_TAIExecuter_DeactivatePlayer_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x4a],iVar3);
          }
          piVar5 = (int *)EventLog_TEventLog_Create_23EDC2EF
                                    (&EventLog__TEndTurnEventLog_E299833A,1,0);
          iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x50],iVar3);
          EventLog_TEventLogbook_AddEvent_23EDC2EF(*(undefined4 *)(iVar4 + 0xd8),piVar5);
          (**(code **)(*piVar5 + 0x2c))();
        }
        Engine_InitMsg_B5A7F841(auStack_40,0x20020007,param_1);
        iStack_28 = iVar3;
        (**(code **)(*AoWE_AoWHSMap_6B957A58 + 0x68))(AoWE_AoWHSMap_6B957A58,auStack_40,param_1);
        iVar3 = iVar3 + 1;
        iVar6 = iVar6 + -1;
      } while (iVar6 != 0);
    }
    AoWE_TAoWHSMap_UnlockRelationsChanged_23EDC2EF(AoWE_AoWHSMap_6B957A58);
    (**(code **)(*param_1 + 0x50))();
  }
  return;
}

