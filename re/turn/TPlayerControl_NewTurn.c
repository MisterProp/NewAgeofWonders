
void AoWE_TPlayerControl_NewTurn_23EDC2EF(undefined4 param_1,char param_2)

{
  undefined1 *puVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  undefined4 *in_FS_OFFSET;
  undefined *puVar5;
  undefined4 uVar6;
  undefined *puVar7;
  undefined *puVar8;
  char *pcVar9;
  undefined4 uVar10;
  undefined4 uVar11;
  undefined4 uVar12;
  undefined4 uStack_68;
  undefined1 *puStack_64;
  undefined1 *puStack_60;
  undefined4 uStack_5c;
  undefined1 *puStack_58;
  undefined1 *puStack_54;
  undefined4 local_44;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38;
  undefined1 local_34 [24];
  int local_1c;
  
                    /* 0x553c4  2546  AoWE.TPlayerControl.NewTurn@23EDC2EF */
  puStack_54 = &stack0xfffffffc;
  local_38 = 0;
  local_3c = 0;
  local_40 = 0;
  local_44 = 0;
  puStack_58 = &LAB_557556f3;
  uStack_5c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_5c;
  puVar1 = &stack0xfffffffc;
  if (AoWE_AoWHSMap_6B957A58[0x5d] != 1) {
    iVar4 = (int)param_2;
    puStack_60 = (undefined1 *)0x55755414;
    iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x50],iVar4);
    puStack_60 = *(undefined1 **)(iVar2 + 0xc4);
    puStack_64 = (undefined1 *)0x5575542d;
    iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x50],iVar4);
    puStack_60 = (undefined1 *)((int)puStack_60 + *(int *)(iVar2 + 0xd0));
    puStack_64 = (undefined1 *)0x55755447;
    AoWE_TPlayerList_GetPlayers_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x50],iVar4);
    puStack_64 = (undefined1 *)0x5575544c;
    iVar2 = AoWE_TPlayer_GetUpkeep_23EDC2EF();
    puStack_60 = (undefined1 *)((int)puStack_60 - iVar2);
    puStack_64 = (undefined1 *)0x55755462;
    uVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x50],iVar4);
    puVar1 = puStack_60;
    puStack_60 = (undefined1 *)0x55755468;
    AoWE_TPlayer_SetGems_23EDC2EF(uVar3,puVar1);
    puStack_60 = (undefined1 *)0x5575547a;
    iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x50],iVar4);
    if (*(int *)(iVar2 + 0xc4) < 0) {
      puStack_60 = (undefined1 *)0x55755495;
      uVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x50],iVar4);
      puStack_60 = (undefined1 *)0x5575549c;
      AoWE_TPlayer_SetGems_23EDC2EF(uVar3,0);
      puStack_60 = (undefined1 *)0x557554ae;
      iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x50],iVar4);
      *(char *)(iVar2 + 0x50) = *(char *)(iVar2 + 0x50) + -5;
      puStack_60 = (undefined1 *)0x557554c4;
      iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x50],iVar4);
      if (*(char *)(iVar2 + 0x50) < -0x32) {
        puStack_60 = (undefined1 *)0x557554dc;
        iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x50],iVar4);
        *(undefined1 *)(iVar2 + 0x50) = 0xce;
      }
    }
    else {
      puStack_60 = (undefined1 *)0x557554f4;
      iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x50],iVar4);
      *(undefined1 *)(iVar2 + 0x50) = 0;
    }
    puStack_60 = (undefined1 *)0x5575550a;
    uVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x50],iVar4);
    puStack_60 = (undefined1 *)0x55755511;
    AoWE_TPlayer_SetEndTurn_23EDC2EF(uVar3,0);
    puVar1 = puStack_54;
  }
  puStack_54 = puVar1;
  puStack_60 = (undefined1 *)0x55755524;
  iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x50],(int)param_2);
  *(int *)(iVar2 + 0x5c) = *(int *)(iVar2 + 0x5c) + 1;
  puStack_60 = (undefined1 *)0x55755531;
  AoWE_TAoWHSMap_LockUpdateVisibility_23EDC2EF(AoWE_AoWHSMap_6B957A58);
  puStack_60 = (undefined1 *)0x55755540;
  Engine_InitMsg_B5A7F841(local_34,0x20020002,param_1);
  local_1c = (int)param_2;
  puStack_60 = (undefined1 *)0x55755555;
  (**(code **)(*AoWE_AoWHSMap_6B957A58 + 0x68))(AoWE_AoWHSMap_6B957A58,local_34,param_1);
  puStack_60 = (undefined1 *)0x55755568;
  Engine_TEventList_TriggerNotifyEvents_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x68],(int)param_2);
  puStack_60 = (undefined1 *)0x55755577;
  Engine_InitMsg_B5A7F841(local_34,0x20020003,param_1);
  local_1c = (int)param_2;
  puStack_60 = (undefined1 *)0x5575558c;
  (**(code **)(*AoWE_AoWHSMap_6B957A58 + 0x68))(AoWE_AoWHSMap_6B957A58,local_34,param_1);
  puStack_60 = (undefined1 *)0x55755596;
  AoWE_TAoWHSMap_UnLockUpdateVisibility_23EDC2EF(AoWE_AoWHSMap_6B957A58);
  if ((char)AoWE_AoWHSMap_6B957A58[0x8e] != '\0') {
    puStack_64 = &LAB_557556ce;
    uStack_68 = *in_FS_OFFSET;
    *in_FS_OFFSET = &uStack_68;
    if (*(char *)((int)AoWE_AoWHSMap_6B957A58 + 0x239) == '\x02') {
      puStack_60 = &stack0xfffffffc;
      if (param_2 == '\0') {
        uVar11 = 0;
        uVar10 = 0;
        iVar2 = AoWE_AoWEngine_7E705837[0xb];
        pcVar9 = "DEBUG\\";
        iVar4 = AoWE_AoWHSMap_6B957A58[0x39];
        puStack_60 = &stack0xfffffffc;
        SysUtils_IntToStr_0F6FDFF6(AoWE_AoWHSMap_6B957A58[0x5d],&local_3c);
        puVar7 = &DAT_5575571c;
        uVar6 = local_3c;
        SysUtils_IntToStr_0F6FDFF6((int)*(char *)((int)AoWE_AoWHSMap_6B957A58 + 0xa5),&local_40);
        puVar5 = &DAT_55755728;
        uVar3 = local_40;
        System__LStrCatN_51F89FF7(&local_38,7);
        (**(code **)(*AoWE_AoWEngine_7E705837 + 0xac))
                  (AoWE_AoWEngine_7E705837,local_38,*AoWE_AoWEngine_7E705837,puVar5,uVar3,puVar7,
                   uVar6,iVar4,pcVar9,iVar2,uVar10,uVar11);
      }
    }
    else {
      uVar12 = 0;
      uVar11 = 0;
      iVar2 = AoWE_AoWEngine_7E705837[0xb];
      pcVar9 = "DEBUG\\";
      iVar4 = AoWE_AoWHSMap_6B957A58[0x39];
      puStack_60 = &stack0xfffffffc;
      SysUtils_IntToStr_0F6FDFF6(AoWE_AoWHSMap_6B957A58[0x5d],&local_3c);
      puVar8 = &DAT_55755738;
      uVar10 = local_3c;
      SysUtils_IntToStr_0F6FDFF6((int)param_2,&local_40);
      puVar7 = &DAT_5575571c;
      uVar6 = local_40;
      SysUtils_IntToStr_0F6FDFF6((int)*(char *)((int)AoWE_AoWHSMap_6B957A58 + 0xa5),&local_44);
      puVar5 = &DAT_55755728;
      uVar3 = local_44;
      System__LStrCatN_51F89FF7(&local_38,9);
      (**(code **)(*AoWE_AoWEngine_7E705837 + 0xac))
                (AoWE_AoWEngine_7E705837,local_38,*AoWE_AoWEngine_7E705837,puVar5,uVar3,puVar7,uVar6
                 ,puVar8,uVar10,iVar4,pcVar9,iVar2,uVar11,uVar12);
    }
    *in_FS_OFFSET = uStack_68;
  }
  puVar1 = puStack_54;
  *in_FS_OFFSET = uStack_5c;
  puStack_54 = &LAB_557556fa;
  puStack_58 = (undefined1 *)0x557556f2;
  System__LStrArrayClr_51F89FF7(&local_44,4,puVar1);
  return;
}

