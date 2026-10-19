// aowepack : AoWE.TAoWHSMap.Create.c
// Ghidra 12.1.2 decompilation of AoWEPACK.dpl (image base per module), function @ 0x557748a4.
// Proves: TAoWHSMap+0x140 = TPlayerList (tag 0x15), +0x144 = TRaceList (tag 0x16).
// Reference for src/engine/map.cpp - see docs/01_FORMATS.md (HSM / CSM / HSS section).


int AoWE_TAoWHSMap_Create_23EDC2EF(int param_1,char param_2)

{
  int iVar1;
  char cVar2;
  undefined4 uVar3;
  int iVar4;
  char extraout_DL;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_54;
  undefined **local_3c;
  undefined **local_38;
  undefined **local_34;
  undefined **local_30;
  undefined **local_2c;
  undefined **local_28;
  undefined **local_24;
  undefined **local_20;
  undefined **local_1c;
  undefined **local_18;
  undefined **local_14;
  undefined **local_10;
  undefined **local_c;
  char local_5;
  
                    /* 0x748a4  1483  AoWE.TAoWHSMap.Create@23EDC2EF */
  local_5 = '\0';
  if (param_2 != '\0') {
    param_1 = System__ClassCreate_51F89FF7();
    local_5 = extraout_DL;
  }
  AoWE_TAbstractAoWHSMap_Create_23EDC2EF(param_1,0);
  uVar3 = AoWE_TItemControl_Create_23EDC2EF(&AoWE__TItemControl_0F7FDBEB,1,0);
  *(undefined4 *)(param_1 + 0xf4) = uVar3;
  iVar4 = NetworkE_TTokenManager_Create_23EDC2EF
                    (&AoWE__TAoWTokenManager_97F65F1B,1,AoWE_AoWEngine_7E705837);
  *(int *)(param_1 + 0x23c) = iVar4;
  Engine_TEventList_Add_23EDC2EF(*(undefined4 *)(*(int *)(iVar4 + 4) + 0x2c));
  iVar4 = *(int *)(param_1 + 0x23c);
  iVar1 = *(int *)(iVar4 + 4);
  *(int *)(iVar1 + 0x44) = param_1;
  *(code **)(iVar1 + 0x40) = AoWE_TAoWHSMap_OnExecuteTokenEvent_23EDC2EF;
  *(int *)(iVar4 + 0x18) = param_1;
  *(code **)(iVar4 + 0x14) = AoWE_TAoWHSMap_OnTokenManagerMessage_23EDC2EF;
  iVar4 = NetworkE_TPlayerCommunicator_Create_23EDC2EF
                    (NetworkE__TPlayerCommunicator_EF4B81E3_exref,
                     CONCAT31((int3)((uint)iVar1 >> 8),1),0);
  *(int *)(param_1 + 0x240) = iVar4;
  *(int *)(iVar4 + 8) = AoWE_AoWEngine_7E705837;
  *(undefined1 *)(param_1 + 0x13a) = 0;
  uVar3 = AoWE_TAIExecuter_Create_23EDC2EF(&AoWE__TAIExecuter_81763FC1,1,0);
  *(undefined4 *)(param_1 + 0x128) = uVar3;
  uVar3 = AoWE_TAIGroupManager_Create_23EDC2EF(&AoWE__TAIGroupManager_E7A4301E,1,0);
  *(undefined4 *)(param_1 + 0x130) = uVar3;
  uVar3 = AoWE_TAIPlayerActionManager_Create_23EDC2EF(&AoWE__TAIPlayerActionManager_71376490,1,0);
  *(undefined4 *)(param_1 + 300) = uVar3;
  local_38 = &AoWE__TEndTurnAIPA_3801BE80;
  local_34 = &AoWE__TGroupControlAIPA_FA6E99B9;
  local_30 = &AoWE__TGroupControlLoopAIPA_5A9275DC;
  local_2c = &AoWE__TInitializeAIPA_36E68B84;
  local_28 = &AoWE__TBudgetManagerAIPA_4F4B9467;
  local_24 = &AoWE__TAutoProduceAIPA_D6B62A6C;
  local_20 = &AoWE__TDiplomacyAIPA_5663D91B;
  local_1c = &AoWE__TProcessEventlogAIPA_53CD0477;
  local_18 = &AoWE__TResearchSpellAIPA_7D18B341;
  local_14 = &AoWE__TCastSpellAIPA_EE6DB8BB;
  local_10 = &AoWE__TStructurePhase1AIPA_3337925E;
  local_c = &AoWE__TStructurePhase2AIPA_42E74C6D;
  AoWE_TAIPlayerActionManager_RegisterClasses_23EDC2EF(uVar3,&local_38,0xb);
  Engine_TEventList_Add_23EDC2EF(*(undefined4 *)(*(int *)(*(int *)(param_1 + 0x23c) + 4) + 0x28));
  local_3c = &AoWE__THuntAG_BF99C8F9;
  AoWE_TAIGroupManager_RegisterAIGroupClasses_23EDC2EF(*(undefined4 *)(param_1 + 0x130),&local_3c,0)
  ;
  *(undefined1 *)(param_1 + 0x239) = 0;
  uVar3 = Engine_TEObject_Create_23EDC2EF(&AoWE__TAoWHSMapKeyboard_D86A8549,1,0);
  *(undefined4 *)(param_1 + 0x134) = uVar3;
  uVar3 = AoWE_TTargetSelectionControl_Create_23EDC2EF(&AoWE__TTargetSelectionControl_DDA2CF6D,1);
  *(undefined4 *)(param_1 + 0xe0) = uVar3;
  uVar3 = System_TObject_Create_23EDC2EF(Classes__TList_9F2060BF_exref,1);
  *(undefined4 *)(param_1 + 0x1f0) = uVar3;
  *(undefined4 *)(param_1 + 0x230) = 0;
  *(undefined1 *)(param_1 + 0x10c) = 5;
  uVar3 = AoWE_TSelectedArmy_Create_23EDC2EF(&AoWE__TSelectedArmy_DCDD069C,1);
  *(undefined4 *)(param_1 + 0xd8) = uVar3;
  Engine_TEventList_Add_23EDC2EF(*(undefined4 *)(*(int *)(param_1 + 0xc) + 0x4c));
  uVar3 = AoWE_TMoveControl_Create_23EDC2EF(&AoWE__TMoveControl_7D8F288F,1,param_1);
  *(undefined4 *)(param_1 + 0x184) = uVar3;
  uVar3 = AoWE_TGlobalMagicControl_Create_23EDC2EF(&AoWE__TGlobalMagicControl_FA34B32D,1,0);
  *(undefined4 *)(param_1 + 0x188) = uVar3;
  System__LStrAsg_51F89FF7(param_1 + 0xe4,"noname");
  System__LStrClr_40929B27(param_1 + 0xe8);
  *(undefined4 *)(param_1 + 0x168) = 0xffffffff;
  *(undefined4 *)(param_1 + 0xec) = 5;
  *(undefined4 *)(param_1 + 0x154) = 5000;
  *(undefined4 *)(param_1 + 0x150) = 5000;
  uVar3 = System_TObject_Create_23EDC2EF(Classes__TStringList_D439CA30_exref,1);
  *(undefined4 *)(param_1 + 0xf0) = uVar3;
  uVar3 = System_TObject_Create_23EDC2EF(Classes__TStringList_D439CA30_exref,1);
  *(undefined4 *)(param_1 + 0x108) = uVar3;
  *(undefined1 *)(param_1 + 0x104) = 2;
  uVar3 = AoWE_TPlayerList_Create_23EDC2EF(&AoWE__TPlayerList_FC6DD6A5,1,0);
  *(undefined4 *)(param_1 + 0x140) = uVar3;
  uVar3 = AoWE_TRaceList_Create_23EDC2EF(&AoWE__TRaceList_FA284442,1,0);
  *(undefined4 *)(param_1 + 0x144) = uVar3;
  uVar3 = System_TObject_Create_23EDC2EF(Engine__TEventList_49A198C7_exref,1);
  *(undefined4 *)(param_1 + 0x198) = uVar3;
  *(undefined1 *)(param_1 + 0x148) = 1;
  *(undefined1 *)(param_1 + 0x14b) = 0;
  *(undefined1 *)(param_1 + 0xa4) = 0;
  *(undefined1 *)(param_1 + 0x248) = 1;
  uVar3 = Engine_TENode_Create_23EDC2EF(&EventLog__TEventLogList_B82DA194,1,0);
  *(undefined4 *)(param_1 + 0x1cc) = uVar3;
  *(undefined1 *)(param_1 + 0x238) = 0;
  uVar3 = AoWE_TTutorialControl_Create_23EDC2EF(&AoWE__TTutorialControl_D622D772,1,0);
  *(undefined4 *)(param_1 + 0x124) = uVar3;
  *(undefined1 *)(param_1 + 0x16c) = 0;
  uVar3 = System_TObject_Create_23EDC2EF(Engine__TByteList_F5602782_exref,1);
  *(undefined4 *)(param_1 + 0x170) = uVar3;
  *(undefined4 *)(param_1 + 0x174) = 0;
  *(undefined1 *)(param_1 + 0x118) = 0;
  *(undefined1 *)(param_1 + 0x119) = 0;
  System__LStrClr_40929B27(param_1 + 0x178);
  cVar2 = AoWReg_TAoWRegistry_GetDetailLevel_23EDC2EF
                    (*(undefined4 *)(AoWE_AoWEngine_7E705837 + 0x5c));
  *(bool *)(param_1 + 0x249) = (byte)(cVar2 - 1U) < 2;
  uVar3 = AoWE_THeroControl_Create_23EDC2EF(&AoWE__THeroControl_6D43E51F,1,0);
  *(undefined4 *)(param_1 + 0xf8) = uVar3;
  uVar3 = AoWE_TUnitControl_Create_23EDC2EF(&AoWE__TUnitControl_1587F19F,1,0);
  *(undefined4 *)(param_1 + 0xfc) = uVar3;
  uVar3 = AoWE_TStructureControl_Create_23EDC2EF(&AoWE__TStructureControl_6055BB9C,1,0);
  *(undefined4 *)(param_1 + 0x100) = uVar3;
  uVar3 = System_TObject_Create_23EDC2EF(Engine__TEventList_49A198C7_exref,1);
  *(undefined4 *)(param_1 + 0x17c) = uVar3;
  uVar3 = Engine_TENode_Create_23EDC2EF(&AoWE__TTextControl_D6A5CDBC,1,0);
  *(undefined4 *)(param_1 + 0x13c) = uVar3;
  uVar3 = System_TObject_Create_23EDC2EF(Engine__TEventList_49A198C7_exref,1);
  *(undefined4 *)(param_1 + 0x19c) = uVar3;
  uVar3 = System_TObject_Create_23EDC2EF(Engine__TEventList_49A198C7_exref,1);
  *(undefined4 *)(param_1 + 0x1a0) = uVar3;
  uVar3 = System_TObject_Create_23EDC2EF(Engine__TEventList_49A198C7_exref,1);
  *(undefined4 *)(param_1 + 0x1a4) = uVar3;
  uVar3 = System_TObject_Create_23EDC2EF(Engine__TEventList_49A198C7_exref,1);
  *(undefined4 *)(param_1 + 0x1a8) = uVar3;
  uVar3 = System_TObject_Create_23EDC2EF(Engine__TEventList_49A198C7_exref,1);
  *(undefined4 *)(param_1 + 0x1ac) = uVar3;
  uVar3 = System_TObject_Create_23EDC2EF(Engine__TEventList_49A198C7_exref,1);
  *(undefined4 *)(param_1 + 0x20c) = uVar3;
  uVar3 = System_TObject_Create_23EDC2EF(Engine__TEventList_49A198C7_exref,1);
  *(undefined4 *)(param_1 + 0x210) = uVar3;
  uVar3 = System_TObject_Create_23EDC2EF(Engine__TEventList_49A198C7_exref,1);
  *(undefined4 *)(param_1 + 0x220) = uVar3;
  uVar3 = System_TObject_Create_23EDC2EF(Engine__TEventList_49A198C7_exref,1);
  *(undefined4 *)(param_1 + 0x208) = uVar3;
  Engine_TEventList_Add_23EDC2EF(uVar3);
  uVar3 = System_TObject_Create_23EDC2EF(Engine__TEventList_49A198C7_exref,1);
  *(undefined4 *)(param_1 + 0x218) = uVar3;
  Engine_TEventList_Add_23EDC2EF(uVar3);
  uVar3 = System_TObject_Create_23EDC2EF(Engine__TEventList_49A198C7_exref,1);
  *(undefined4 *)(param_1 + 0x21c) = uVar3;
  uVar3 = System_TObject_Create_23EDC2EF(Engine__TEventList_49A198C7_exref,1);
  *(undefined4 *)(param_1 + 0x214) = uVar3;
  uVar3 = System_TObject_Create_23EDC2EF(Engine__TEventList_49A198C7_exref,1);
  *(undefined4 *)(param_1 + 0x1b0) = uVar3;
  uVar3 = System_TObject_Create_23EDC2EF(Engine__TEventList_49A198C7_exref,1);
  *(undefined4 *)(param_1 + 0x1d0) = uVar3;
  uVar3 = EventLog_TExecuteEventLogCallBackList_Create_23EDC2EF
                    (&EventLog__TExecuteEventLogCallBackList_BED2CE8F,1);
  *(undefined4 *)(param_1 + 0x1d4) = uVar3;
  uVar3 = System_TObject_Create_23EDC2EF(Engine__TEventList_49A198C7_exref,1);
  *(undefined4 *)(param_1 + 0x1e0) = uVar3;
  uVar3 = System_TObject_Create_23EDC2EF(Engine__TEventList_49A198C7_exref,1);
  *(undefined4 *)(param_1 + 0x1e4) = uVar3;
  uVar3 = AoWE_TStartupSongList_Create_23EDC2EF(&AoWE__TStartupSongList_00CA496F,1,0);
  *(undefined4 *)(param_1 + 0x110) = uVar3;
  uVar3 = Sound_TSonglist_Create_23EDC2EF(Sound__TSonglist_F897E00E_exref,1,0);
  *(undefined4 *)(param_1 + 0x114) = uVar3;
  uVar3 = System_TObject_Create_23EDC2EF(Engine__TEventList_49A198C7_exref,1);
  *(undefined4 *)(param_1 + 0x1b4) = uVar3;
  uVar3 = System_TObject_Create_23EDC2EF(Engine__TEventList_49A198C7_exref,1);
  *(undefined4 *)(param_1 + 0x1b8) = uVar3;
  uVar3 = System_TObject_Create_23EDC2EF(Engine__TEventList_49A198C7_exref,1);
  *(undefined4 *)(param_1 + 0x1bc) = uVar3;
  uVar3 = System_TObject_Create_23EDC2EF(Engine__TEventList_49A198C7_exref,1);
  *(undefined4 *)(param_1 + 0x1e8) = uVar3;
  uVar3 = System_TObject_Create_23EDC2EF(Engine__TEventList_49A198C7_exref,1);
  *(undefined4 *)(param_1 + 0x1ec) = uVar3;
  uVar3 = System_TObject_Create_23EDC2EF(Engine__TEventList_49A198C7_exref,1);
  *(undefined4 *)(param_1 + 0x224) = uVar3;
  uVar3 = System_TObject_Create_23EDC2EF(Engine__TEventList_49A198C7_exref,1);
  *(undefined4 *)(param_1 + 0x228) = uVar3;
  uVar3 = System_TObject_Create_23EDC2EF(Engine__TEventList_49A198C7_exref,1);
  *(undefined4 *)(param_1 + 0x1c4) = uVar3;
  uVar3 = AoWE_TAoWMapCampaignSettings_Create_23EDC2EF(&AoWE__TAoWMapCampaignSettings_A2FF471E,1,0);
  *(undefined4 *)(param_1 + 0x18c) = uVar3;
  Engine_TEventList_Add_23EDC2EF(*(undefined4 *)(*(int *)(param_1 + 0xc) + 0x1c));
  if (local_5 != '\0') {
    *in_FS_OFFSET = uStack_54;
  }
  return param_1;
}

