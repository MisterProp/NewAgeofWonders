

void AoWE_TTurnPlayerControl_NewTurn_23EDC2EF(int *param_1,undefined4 param_2)



{

  undefined1 *puVar1;

  char cVar2;

  int iVar3;

  undefined4 uVar4;

  int *piVar5;

  char cVar6;

  int iVar7;

  undefined4 *in_FS_OFFSET;

  undefined4 *puVar8;

  undefined4 *puVar9;

  undefined4 uStack_40;

  undefined1 *puStack_3c;

  undefined1 *puStack_38;

  undefined4 local_28;

  undefined4 local_24;

  undefined1 local_20;

  undefined4 local_1c;

  undefined4 local_18;

  undefined4 local_14;

  int *local_10;

  int *local_c;

  int *local_8;

  

                    /* 0x56714  2521  AoWE.TTurnPlayerControl.NewTurn@23EDC2EF */

  puStack_38 = &stack0xfffffffc;

  local_18 = 0;

  local_1c = 0;

  local_28 = 0;

  puStack_3c = &LAB_55756c58;

  uStack_40 = *in_FS_OFFSET;

  *in_FS_OFFSET = &uStack_40;

  local_8 = param_1;

  AoWE_TAbstractAoWHSMap_LockMapChangedEvent_23EDC2EF(AoWE_AoWHSMap_6B957A58);

  cVar6 = (char)param_2;

  iVar7 = (int)cVar6;

  NetworkE_TTokenExecuter_ActivateTC_23EDC2EF

            (*(undefined4 *)(AoWE_AoWHSMap_6B957A58[0x8f] + 4),iVar7);

  if (*(char *)((int)AoWE_AoWHSMap_6B957A58 + 0x139) == '\0') {

    iVar7 = AoWE_TPlayerList_GetPlayers_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x50],(int)cVar6);

    if ((*(char *)(iVar7 + 0xa7) == '\0') &&

       (cVar6 == *(char *)((int)AoWE_AoWHSMap_6B957A58 + 0xa5))) {

      if ((*(char *)((int)AoWE_AoWHSMap_6B957A58 + 0x11a) == '\x02') &&

         (AoWE_AoWHSMap_6B957A58[0x5d] == 1)) {

        AoWE_TTutorialControl_ShowMessage_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x49],0x32,0,1);

      }

      else {

        AoWE_TAoWHSMap_ResetTurnTimer_23EDC2EF(AoWE_AoWHSMap_6B957A58);

        AoWE_TAoWHSMap_EnableTurnTimer_23EDC2EF(AoWE_AoWHSMap_6B957A58);

        AoWE_TAoWHSMap_ResetSeatedPlayerView_23EDC2EF(AoWE_AoWHSMap_6B957A58);

        local_c = (int *)EventLog_TEventLog_Create_23EDC2EF(&AoWE__TNewTurnEventLog_98FAA535,1,0);

        *(char *)(local_c + 6) = cVar6;

        *(undefined1 *)(local_c + 8) = 0;

        *(undefined1 *)((int)local_c + 0x21) = 2;

        System_LoadResString_02614E1B(&AoWE_ItsYourTurnRStr_8276FCC3,&local_1c);

        AoWE_TranslateRStr_435799AA(local_1c,&local_18);

        System__LStrAsg_51F89FF7(local_c + 7,local_18);

        (**(code **)(*AoWE_AoWHSMap_6B957A58 + 300))(AoWE_AoWHSMap_6B957A58,local_c,0,0,0);

        (**(code **)(*local_c + 0x2c))();

        AoWE_TTurnPlayerControl_ReplayLogbook_23EDC2EF(local_8);

      }

    }

  }

  else {

    if (*(char *)((int)AoWE_AoWHSMap_6B957A58 + 0xa5) != -1) {

      (**(code **)(*AoWE_AoWHSMap_6B957A58 + 0xf8))(AoWE_AoWHSMap_6B957A58,0xffffffff);

    }

    iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x50],iVar7);

    if (*(char *)(iVar3 + 0xa7) == '\0') {

      local_c = (int *)EventLog_TEventLog_Create_23EDC2EF(&AoWE__TNewTurnEventLog_98FAA535,1,0);

      *(char *)(local_c + 6) = (char)AoWE_AoWHSMap_6B957A58[0x29];

      *(undefined1 *)(local_c + 8) = 0;

      *(undefined1 *)((int)local_c + 0x21) = 2;

      System_LoadResString_02614E1B(&AoWE_ItsYourTurnRStr_8276FCC3,&local_1c);

      AoWE_TranslateRStr_435799AA(local_1c,&local_18);

      System__LStrAsg_51F89FF7(local_c + 7,local_18);

      (**(code **)(*AoWE_AoWHSMap_6B957A58 + 300))

                (AoWE_AoWHSMap_6B957A58,local_c,0,

                 AoWE_TTurnPlayerControl_ShowNewTurnCallBack_23EDC2EF,local_8);

      (**(code **)(*local_c + 0x2c))();

    }

    else {

      local_c = (int *)EventLog_TEventLog_Create_23EDC2EF(&AoWE__TNewTurnEventLog_98FAA535,1,0);

      *(char *)(local_c + 6) = (char)AoWE_AoWHSMap_6B957A58[0x29];

      *(undefined1 *)(local_c + 8) = 0;

      *(undefined1 *)((int)local_c + 0x21) = 0;

      if (cVar6 == '\0') {

        System_LoadResString_02614E1B(&AoWE_IndependentsAreMovingRStr_8276FCC3,&local_1c);

        AoWE_TranslateRStr_435799AA(local_1c,&local_18);

        System__LStrAsg_51F89FF7(local_c + 7,local_18);

      }

      else {

        puVar9 = &local_18;

        uVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x50],iVar7);

        AoWE_TPlayer_GetName_23EDC2EF(uVar4,&local_1c);

        local_24 = local_1c;

        local_20 = 0xb;

        puVar8 = &local_24;

        System_LoadResString_02614E1B(&AoWE_PlayerIsMovingRStr_8276FCC3,&local_18);

        AoWE_TranslateRStr_435799AA(local_18,&local_28);

        AoWE_Format_D8F48273(local_28,puVar8,0,puVar9);

        System__LStrAsg_51F89FF7(local_c + 7,local_18);

      }

      (**(code **)(*AoWE_AoWHSMap_6B957A58 + 300))(AoWE_AoWHSMap_6B957A58,local_c,1,0,0);

      (**(code **)(*local_c + 0x2c))();

    }

  }

  AoWE_TPlayerControl_NewTurn_23EDC2EF(local_8,param_2);

  iVar3 = (int)cVar6;

  iVar7 = AoWE_TPlayerList_GetPlayers_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x50],iVar3);

  if ((*(char *)(iVar7 + 0xa7) == '\0') &&

     (*(char *)((int)AoWE_AoWHSMap_6B957A58 + 0x13a) != '\x02')) {

    cVar2 = AoWReg_TAoWRegistry_GetAutoSave_23EDC2EF

                      (*(undefined4 *)(AoWE_AoWEngine_7E705837 + 0x5c));

    if (cVar2 != '\0') {

      (**(code **)(*local_8 + 0x58))();

    }

  }

  AoWE_TAbstractAoWHSMap_UnlockMapChangedEvent_23EDC2EF(AoWE_AoWHSMap_6B957A58);

  AoWE_TAbstractAoWHSMap_InvalidateMap_23EDC2EF(AoWE_AoWHSMap_6B957A58);

  iVar7 = AoWE_TPlayerList_GetPlayers_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x50],iVar3);

  if (*(char *)(iVar7 + 0xa7) == '\0') {

    Sound_PlaySample("SFX\\EVENTS\\NEWTURN.WAV");

    if ((AoWE_AoWHSMap_6B957A58[0x5d] == 1) &&

       ((*(char *)((int)AoWE_AoWHSMap_6B957A58 + 0x139) != '\0' ||

        (cVar6 == *(char *)((int)AoWE_AoWHSMap_6B957A58 + 0xa5))))) {

      if ((*(char *)((int)AoWE_AoWHSMap_6B957A58 + 0x11a) == '\0') ||

         (*(char *)((int)AoWE_AoWHSMap_6B957A58 + 0x11a) == '\x02')) {

        local_10 = (int *)EventLog_TEventLog_Create_23EDC2EF

                                    (&EventLog__TShowBookEventLog_1A9F869D,1,0);

        (**(code **)(*AoWE_AoWHSMap_6B957A58 + 300))(AoWE_AoWHSMap_6B957A58,local_10,0,0,0);

        (**(code **)(*local_10 + 0x2c))();

      }

      iVar7 = AoWE_TPlayerList_GetPlayers_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x50],(int)cVar6);

      if (*(int *)(*(int *)(iVar7 + 0x54) + 0x34) == 0) {

        local_14 = AoWE_TSpellList_Create_23EDC2EF(&AoWE__TSpellList_8F695673,1);

        iVar7 = AoWE_TPlayerList_GetPlayers_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x50],(int)cVar6);

        AoWE_TPlayerMagicControl_ListResearchSpells_23EDC2EF

                  (*(undefined4 *)(iVar7 + 0x54),&local_14,7);

        iVar7 = AoWE_TSpellList_GetCount_23EDC2EF(local_14);

        if (0 < iVar7) {

          piVar5 = (int *)AoWE_TPlayerMagicEventLog_Create_23EDC2EF

                                    (&AoWE__TPlayerMagicEventLog_A81505B3,1,0);

          *(undefined1 *)(piVar5 + 6) = 2;

          (**(code **)(*AoWE_AoWHSMap_6B957A58 + 300))(AoWE_AoWHSMap_6B957A58,piVar5,0,0,0);

          (**(code **)(*piVar5 + 0x2c))();

        }

        System_TObject_Free_23EDC2EF(local_14);

      }

      AoWE_TTutorialControl_ShowMessage_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x49],0x34,0,1);

    }

  }

  else {

    AoWE_TAIExecuter_ActivatePlayer_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x4a],param_2);

    if (cVar6 == '\0') {

      puVar9 = &local_18;

      System_LoadResString_02614E1B(&AoWE_IndependentsRStr_8276FCC3,&local_18);

      AoWE_TranslateRStr_435799AA(local_18,&local_1c);

      local_24 = local_1c;

      local_20 = 0xb;

      puVar8 = &local_24;

      System_LoadResString_02614E1B(&AoWE_AreMovingRStr_8276FCC3,&local_18);

      AoWE_TranslateRStr_435799AA(local_18,&local_28);

      AoWE_Format_D8F48273(local_28,puVar8,0,puVar9);

      (**(code **)(*AoWE_AoWHSMap_6B957A58 + 0x110))(AoWE_AoWHSMap_6B957A58,local_18);

    }

    else {

      puVar9 = &local_18;

      iVar7 = AoWE_TPlayerList_GetPlayers_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x50],iVar3);

      AoWE_TranslateRStr_435799AA

                (*(undefined4 *)(&AoWE_RaceNamesPluralRStr_B11890CE + *(char *)(iVar7 + 0xa5) * 4),

                 &local_1c);

      local_24 = local_1c;

      local_20 = 0xb;

      puVar8 = &local_24;

      System_LoadResString_02614E1B(&AoWE_AreMovingRStr_8276FCC3,&local_18);

      AoWE_TranslateRStr_435799AA(local_18,&local_28);

      AoWE_Format_D8F48273(local_28,puVar8,0,puVar9);

      (**(code **)(*AoWE_AoWHSMap_6B957A58 + 0x110))(AoWE_AoWHSMap_6B957A58,local_18);

    }

  }

  puVar1 = puStack_38;

  *in_FS_OFFSET = uStack_40;

  puStack_38 = &LAB_55756c5f;

  puStack_3c = (undefined1 *)0x55756c4a;

  System__LStrClr_40929B27(&local_28,uStack_40,puVar1);

  puStack_3c = (undefined1 *)0x55756c57;

  System__LStrArrayClr_51F89FF7(&local_1c,2);

  return;

}




