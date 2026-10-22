
int AoWE_TPlayer_Create_23EDC2EF(int param_1,char param_2)

{
  undefined4 uVar1;
  int iVar2;
  char extraout_DL;
  char cVar3;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_20;
  
                    /* 0x50ea8  2642  AoWE.TPlayer.Create@23EDC2EF */
  cVar3 = '\0';
  if (param_2 != '\0') {
    param_1 = System__ClassCreate_51F89FF7();
    cVar3 = extraout_DL;
  }
  Engine_TEObject_Create_23EDC2EF(param_1,0);
  uVar1 = System_TObject_Create_23EDC2EF(Engine__TEventList_49A198C7_exref,1);
  *(undefined4 *)(param_1 + 0x10) = uVar1;
  uVar1 = System_TObject_Create_23EDC2EF(Engine__TEventList_49A198C7_exref,1);
  *(undefined4 *)(param_1 + 0xc) = uVar1;
  uVar1 = System_TObject_Create_23EDC2EF(Engine__TEventList_49A198C7_exref,1);
  *(undefined4 *)(param_1 + 0x14) = uVar1;
  iVar2 = Engine_TEChangeNotifyEventList_Create_23EDC2EF(&AoWE__TPlayerStructureList_203F7D17,1,0);
  *(int *)(param_1 + 0x1c) = iVar2;
  *(int *)(iVar2 + 0x14) = param_1;
  uVar1 = Engine_TEChangeNotifyEventList_Create_23EDC2EF(&AoWE__TArmyList_42A69BF2,1,0);
  *(undefined4 *)(param_1 + 0x28) = uVar1;
  iVar2 = AoWE_TAIPlayerControl_Create_23EDC2EF(&AoWE__TAIPlayerControl_0AB27005,1,0);
  *(int *)(param_1 + 0x88) = iVar2;
  *(int *)(iVar2 + 8) = param_1;
  iVar2 = AoWE_TAIBudgetManager_Create_23EDC2EF(&AoWE__TAIBudgetManager_06A1DFED,1,0);
  *(int *)(param_1 + 0x8c) = iVar2;
  *(int *)(iVar2 + 8) = param_1;
  uVar1 = Engine_TENode_Create_23EDC2EF(&AoWE__TAIGroupList_F96D75E2,1,0);
  *(undefined4 *)(param_1 + 0x84) = uVar1;
  iVar2 = AoWE_TPlayerMagicControl_Create_23EDC2EF(&AoWE__TPlayerMagicControl_FA626B4C,1,0);
  *(int *)(param_1 + 0x54) = iVar2;
  *(int *)(iVar2 + 0x10) = param_1;
  uVar1 = AoWE_TPlayerStatistics_Create_23EDC2EF(&AoWE__TPlayerStatistics_16044818,1,0);
  *(undefined4 *)(param_1 + 0x60) = uVar1;
  iVar2 = *(int *)(param_1 + 0x28);
  *(int *)(iVar2 + 0x14) = param_1;
  Engine_TEventList_Add_23EDC2EF(*(undefined4 *)(iVar2 + 0x10));
  uVar1 = Engine_TENode_Create_23EDC2EF(&AoWE__THeroList_9E38D53D,1,0);
  *(undefined4 *)(param_1 + 0x80) = uVar1;
  *(undefined4 *)(param_1 + 0xac) = 0xfa;
  *(undefined4 *)(param_1 + 0xb0) = 0;
  *(undefined1 *)(param_1 + 0xa6) = 0xff;
  *(undefined1 *)(param_1 + 0xa9) = 0;
  uVar1 = EventLog_TEventLogbook_Create_23EDC2EF(&EventLog__TEventLogbook_B82F4E43,1,0);
  *(undefined4 *)(param_1 + 0xd8) = uVar1;
  EventLog_TEventLogbook_SetOnChanged_23EDC2EF(uVar1);
  uVar1 = System_TObject_Create_23EDC2EF(Engine__TEventList_49A198C7_exref,1);
  *(undefined4 *)(param_1 + 0x18) = uVar1;
  iVar2 = AoWE_TPlayerDiplomaticRelations_Create_23EDC2EF
                    (&AoWE__TPlayerDiplomaticRelations_304E6961,1,0);
  *(int *)(param_1 + 0x40) = iVar2;
  *(int *)(iVar2 + 8) = param_1;
  uVar1 = AoWE_TDiplomaticRelationList_Create_23EDC2EF(&AoWE__TDiplomaticRelationList_3257CE63,1);
  *(undefined4 *)(param_1 + 0x3c) = uVar1;
  uVar1 = System_TObject_Create_23EDC2EF(&AoWE__TProductionControlList_FE02D77D,1);
  *(undefined4 *)(param_1 + 0x24) = uVar1;
  uVar1 = Engine_TENode_Create_23EDC2EF(&AoWE__TIncomeSourceList_4A279CCD,1,0);
  *(undefined4 *)(param_1 + 100) = uVar1;
  uVar1 = AoWE_TLeader_Create_23EDC2EF(&AoWE__TLeader_DF642EA6,1,0);
  *(undefined4 *)(param_1 + 0x58) = uVar1;
  *(undefined1 *)(param_1 + 0x45) = 0;
  *(undefined1 *)(param_1 + 0x44) = 0;
  iVar2 = System_TObject_Create_23EDC2EF(Engine__TIntegerList_D8218C48_exref,1);
  *(int *)(param_1 + 0x98) = iVar2;
  *(undefined4 *)(iVar2 + 0x10) = 0xffffffff;
  iVar2 = System_TObject_Create_23EDC2EF(Engine__TIntegerList_D8218C48_exref,1);
  *(int *)(param_1 + 0x9c) = iVar2;
  *(undefined4 *)(iVar2 + 0x10) = 0xffffffff;
  uVar1 = Engine_TEChangeNotifyEventList_Create_23EDC2EF(&AoWE__TPlayerStructureList_203F7D17,1,0);
  *(undefined4 *)(param_1 + 0x20) = uVar1;
  uVar1 = Engine_TEChangeNotifyEventList_Create_23EDC2EF(&AoWE__TPlayerStructureList_203F7D17,1,0);
  *(undefined4 *)(param_1 + 0x68) = uVar1;
  uVar1 = System_TObject_Create_23EDC2EF(Classes__TStringList_D439CA30_exref,1);
  *(undefined4 *)(param_1 + 0x34) = uVar1;
  uVar1 = Engine_TENode_Create_23EDC2EF(&AoWE__TDiplomaticActionList_3007A8D9,1,0);
  *(undefined4 *)(param_1 + 0x90) = uVar1;
  uVar1 = System_TObject_Create_23EDC2EF(Engine__TBitList_7F8938B7_exref,1);
  *(undefined4 *)(param_1 + 0x94) = uVar1;
  uVar1 = AoWE_TPBEMPlayerSettings_Create_23EDC2EF(&AoWE__TPBEMPlayerSettings_4CAB4812,1,0);
  *(undefined4 *)(param_1 + 0xa0) = uVar1;
  uVar1 = AoWE_TDiplomaticRelationList_Create_23EDC2EF(&AoWE__TDiplomaticRelationList_3257CE63,1);
  *(undefined4 *)(param_1 + 0x38) = uVar1;
  *(undefined1 *)(param_1 + 0xa4) = 0;
  *(undefined4 *)(param_1 + 0xb4) = 0xffffffff;
  *(undefined4 *)(param_1 + 0xb8) = 0xffffffff;
  *(undefined1 *)(param_1 + 0xa5) = 0xff;
  *(undefined1 *)(param_1 + 0x2d) = 0xff;
  *(undefined1 *)(param_1 + 0x2e) = 0xff;
  if (cVar3 != '\0') {
    *in_FS_OFFSET = uStack_20;
  }
  return param_1;
}

