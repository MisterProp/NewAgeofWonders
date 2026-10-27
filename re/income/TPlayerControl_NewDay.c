

void AoWE_TPlayerControl_NewDay_23EDC2EF(undefined4 param_1)



{

  undefined4 *puVar1;

  undefined1 *puVar2;

  int iVar3;

  int *piVar4;

  int iVar5;

  int iVar6;

  undefined4 *in_FS_OFFSET;

  undefined4 uStack_6c;

  undefined *puStack_68;

  undefined4 *puStack_64;

  undefined4 uStack_60;

  undefined1 *puStack_5c;

  undefined1 *puStack_58;

  int local_48;

  undefined1 local_44;

  undefined4 local_40;

  undefined4 local_3c;

  undefined1 local_38 [48];

  undefined4 local_8;

  

                    /* 0x54da4  2549  AoWE.TPlayerControl.NewDay@23EDC2EF */

  puStack_58 = &stack0xfffffffc;

  local_3c = 0;

  local_40 = 0;

  puStack_5c = &LAB_55755100;

  uStack_60 = *in_FS_OFFSET;

  *in_FS_OFFSET = &uStack_60;

  AoWE_AoWHSMap_6B957A58[0x5d] = AoWE_AoWHSMap_6B957A58[0x5d] + 1;

  puStack_64 = (undefined4 *)0x55754de1;

  local_8 = param_1;

  AoWE_TPlayerList_BeginUpdate_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x50]);

  puStack_64 = (undefined4 *)0x55754deb;

  AoWE_TAoWHSMap_LockRelationsChanged_23EDC2EF(AoWE_AoWHSMap_6B957A58);

  puStack_64 = (undefined4 *)0x55754dfb;

  iVar3 = (**(code **)(*(int *)AoWE_AoWHSMap_6B957A58[0x50] + 0x54))();

  iVar3 = iVar3 + -1;

  if (0 < iVar3) {

    iVar6 = 1;

    do {

      puStack_64 = (undefined4 *)0x55754e19;

      piVar4 = (int *)EventLog_TEventLog_Create_23EDC2EF(&EventLog__TNewDayEventLog_E5A93FCA,1,0);

      puStack_64 = &local_3c;

      puStack_68 = (undefined *)0x55754e2c;

      System_LoadResString_02614E1B(&AoWE_NewDayRStr_8276FCC3,&local_3c);

      puStack_68 = (undefined *)0x55754e37;

      AoWE_TranslateRStr_435799AA(local_3c,&local_40);

      local_48 = AoWE_AoWHSMap_6B957A58[0x5d];

      local_44 = 0;

      puStack_68 = (undefined *)0x55754e57;

      AoWE_Format_D8F48273(local_40,&local_48,0);

      puStack_64 = (undefined4 *)0x55754e62;

      System__LStrAsg_51F89FF7(piVar4 + 6,local_3c);

      if ((AoWE_AoWHSMap_6B957A58[0x56] != 0) &&

         (AoWE_AoWHSMap_6B957A58[0x56] - AoWE_AoWHSMap_6B957A58[0x5d] < 0xb)) {

        puStack_64 = (undefined4 *)piVar4[6];

        puStack_68 = &DAT_55755118;

        uStack_6c = 0x55754eb3;

        AoWE_FmtDaysLeftStr_0F6FDFF6

                  (AoWE_AoWHSMap_6B957A58[0x56] - AoWE_AoWHSMap_6B957A58[0x5d],&local_3c);

        uStack_6c = local_3c;

        System__LStrCatN_51F89FF7(piVar4 + 6,4);

      }

      *(char *)(piVar4 + 4) = (char)iVar6;

      puStack_64 = (undefined4 *)0x55754edf;

      iVar5 = AoWE_TPlayerList_GetPlayers_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x50],iVar6);

      puStack_64 = (undefined4 *)0x55754eec;

      EventLog_TEventLogbook_AddEvent_23EDC2EF(*(undefined4 *)(iVar5 + 0xd8),piVar4);

      puStack_64 = (undefined4 *)0x55754ef3;

      (**(code **)(*piVar4 + 0x2c))();

      iVar6 = iVar6 + 1;

      iVar3 = iVar3 + -1;

    } while (iVar3 != 0);

  }

  if ((char)AoWE_AoWHSMap_6B957A58[0x5b] == '\x02') {

    puStack_64 = (undefined4 *)0x55754f13;

    AoWE_TAoWHSMap_GenerateNewTurnOrder_23EDC2EF(AoWE_AoWHSMap_6B957A58);

  }

  if (AoWE_AoWHSMap_6B957A58[0x5d] == 1) {

    puStack_64 = (undefined4 *)0x55754f31;

    AoWE_TRaceList_ResetPlayerRelations_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x51]);

    if (*(char *)((int)AoWE_AoWHSMap_6B957A58 + 0x13a) == '\0') {

      puStack_64 = (undefined4 *)0x55754f44;

      System_Randomize_51F89FF7();

      AoWE_AoWHSMap_6B957A58[0x8b] = *(int *)System_RandSeed_A0665B6E_exref;

    }

    AoWE_AoWHSMap_6B957A58[0x8c] = AoWE_AoWHSMap_6B957A58[0x8b];

  }

  puStack_68 = &LAB_55754fa6;

  uStack_6c = *in_FS_OFFSET;

  *in_FS_OFFSET = &uStack_6c;

  puStack_64 = (undefined4 *)&stack0xfffffffc;

  Engine_InitMsg_B5A7F841(local_38,0x20020001,local_8);

  (**(code **)(*AoWE_AoWHSMap_6B957A58 + 0x68))(AoWE_AoWHSMap_6B957A58,local_38,local_8);

  puVar1 = puStack_64;

  *in_FS_OFFSET = uStack_6c;

  puStack_68 = &LAB_55754fdb;

  uStack_6c = *in_FS_OFFSET;

  *in_FS_OFFSET = &uStack_6c;

  puStack_64 = (undefined4 *)&stack0xfffffffc;

  Engine_TEventList_TriggerNotifyEvents_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x67],local_8,puVar1);

  *in_FS_OFFSET = uStack_6c;

  if (AoWE_AoWHSMap_6B957A58[0x56] == AoWE_AoWHSMap_6B957A58[0x5d]) {

    puStack_64 = (undefined4 *)0x55755010;

    piVar4 = (int *)EventLog_TMessageEventLog_Create_23EDC2EF

                              (&EventLog__TMessageEventLog_3CE3C8DE,

                               CONCAT31((int3)((uint)AoWE_AoWHSMap_6B957A58 >> 8),1),0);

    puStack_64 = (undefined4 *)0x5575501f;

    System_LoadResString_02614E1B(&AoWE_InfoRStr_8276FCC3,&local_40);

    puStack_64 = (undefined4 *)0x5575502a;

    AoWE_TranslateRStr_435799AA(local_40,&local_3c);

    puStack_64 = (undefined4 *)0x55755035;

    System__LStrAsg_51F89FF7(piVar4 + 6,local_3c);

    *(undefined1 *)((int)piVar4 + 0x25) = 0;

    *(undefined1 *)(piVar4 + 9) = 0;

    puStack_64 = (undefined4 *)0x5575504a;

    System_LoadResString_02614E1B(&AoWE_TurnLimitReachedRStr_8276FCC3,&local_40);

    puStack_64 = (undefined4 *)0x55755055;

    AoWE_TranslateRStr_435799AA(local_40,&local_3c);

    puStack_64 = (undefined4 *)0x55755060;

    (**(code **)(*(int *)piVar4[8] + 0x2c))((int *)piVar4[8],local_3c);

    puStack_64 = (undefined4 *)0x0;

    puStack_68 = (undefined1 *)0x0;

    uStack_6c = 0x55755075;

    (**(code **)(*AoWE_AoWHSMap_6B957A58 + 300))(AoWE_AoWHSMap_6B957A58,piVar4,0);

    uStack_6c = 0x5575507c;

    (**(code **)(*piVar4 + 0x2c))();

    uStack_6c = 0x55755093;

    AoWE_TAoWHSMap_GameOver_23EDC2EF

              (AoWE_AoWHSMap_6B957A58,*(undefined1 *)((int)AoWE_AoWHSMap_6B957A58 + 0xa5),2);

    uStack_6c = 0x557550a3;

    iVar3 = (**(code **)(*(int *)AoWE_AoWHSMap_6B957A58[0x50] + 0x54))();

    iVar3 = iVar3 + -1;

    if (0 < iVar3) {

      iVar6 = 1;

      do {

        puStack_64 = (undefined4 *)0x557550bd;

        AoWE_TAoWHSMap_GameOver_23EDC2EF(AoWE_AoWHSMap_6B957A58,iVar6,2);

        iVar6 = iVar6 + 1;

        iVar3 = iVar3 + -1;

      } while (iVar3 != 0);

    }

  }

  puStack_64 = (undefined4 *)0x557550cb;

  AoWE_TAoWHSMap_UnlockRelationsChanged_23EDC2EF(AoWE_AoWHSMap_6B957A58);

  puStack_64 = (undefined4 *)0x557550d5;

  AoWE_TAoWHSMap_TriggerRelationsChanged_23EDC2EF(AoWE_AoWHSMap_6B957A58);

  puStack_64 = (undefined4 *)0x557550e5;

  AoWE_TPlayerList_EndUpdate_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x50]);

  puVar2 = puStack_58;

  *in_FS_OFFSET = uStack_60;

  puStack_58 = &LAB_55755107;

  puStack_5c = (undefined1 *)0x557550ff;

  System__LStrArrayClr_51F89FF7(&local_40,2,puVar2);

  return;

}




