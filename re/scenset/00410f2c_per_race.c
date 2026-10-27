// per_race (0x00410f2c) - decompiled via GhidraMCP


void FUN_00410f2c(int param_1,int param_2,int *param_3,int *param_4,int *param_5,int *param_6,
                 int *param_7,int *param_8)

{
  undefined1 *puVar1;
  char cVar2;
  int iVar3;
  undefined4 uVar4;
  code *pcVar5;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_38;
  undefined1 *puStack_34;
  undefined1 *puStack_30;
  undefined4 local_20;
  undefined4 local_1c;
  int local_18;
  char local_12;
  char local_11;
  char local_10 [4];
  int local_c;
  int local_8;
  
  puStack_30 = &stack0xfffffffc;
  local_1c = 0;
  local_20 = 0;
  puStack_34 = &LAB_00411450;
  uStack_38 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_38;
  local_8 = param_1;
  if (param_2 == 0) {
    (**(code **)(*param_7 + 0x6c))(param_7,0);
    (**(code **)(*param_3 + 0x6c))(param_3,0);
    (**(code **)(*param_8 + 0x6c))(param_8,0);
    (**(code **)(*param_6 + 0x6c))(param_6,0);
    (**(code **)(*param_5 + 0x6c))(param_5,1);
    System_LoadResString_02614E1B(AoWE_UnavailableRStr_04ECFCC5_exref,&local_20);
    AoWE_TranslateRStr_435799AA(local_20,&local_1c);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(param_5,local_1c);
    (**(code **)(*param_4 + 0x6c))(param_4,1);
  }
  else {
    puStack_30 = &stack0xfffffffc;
    cVar2 = AoWSetup_TSetupControl_GetHost_23EDC2EF(*(undefined4 *)(param_1 + 0x33c));
    if ((cVar2 == '\0') || ((*(byte *)(param_2 + 0x19) & 1) != 0)) {
      cVar2 = '\0';
    }
    else {
      cVar2 = '\x01';
    }
    if (cVar2 != *(char *)((int)param_3 + 0x75)) {
      (**(code **)(*param_3 + 0x6c))(param_3,cVar2);
    }
    if (*(char *)((int)param_3 + 0x75) == '\0') {
      (**(code **)(*param_6 + 0x6c))(param_6,1);
      if ((*(char *)(param_2 + 8) == '\0') && (*(int *)(param_2 + 0xc) != 0)) {
        iVar3 = AoWSetup_TRemotePlayerList_IndexOfRemoteID_23EDC2EF
                          (*(undefined4 *)(*(int *)(*(int *)(local_8 + 0x33c) + 0x28) + 0x38),
                           *(undefined4 *)(param_2 + 0xc));
        if (iVar3 == -1) {
          System_LoadResString_02614E1B(AoWE_HumanRStr_04ECFCC5_exref,&local_20);
          AoWE_TranslateRStr_435799AA(local_20,&local_1c);
          AoWLabel_TAOWLabel_SetGText_23EDC2EF(param_6,local_1c);
        }
        else {
          iVar3 = AoWSetup_TRemotePlayerList_GetPlayers_23EDC2EF
                            (*(undefined4 *)(*(int *)(*(int *)(local_8 + 0x33c) + 0x28) + 0x38),
                             iVar3);
          AoWLabel_TAOWLabel_SetGText_23EDC2EF(param_6,*(undefined4 *)(iVar3 + 8));
        }
      }
      else {
        AoWE_TranslateRStr_435799AA
                  (*(undefined4 *)
                    (AoWE_TPlayerTypeRStr_5085FC63_exref + (uint)*(byte *)(param_2 + 8) * 4),
                   &local_1c);
        AoWLabel_TAOWLabel_SetGText_23EDC2EF(param_6,local_1c);
      }
    }
    else {
      Classes_TStrings_BeginUpdate_23EDC2EF(param_3[0x47]);
      (**(code **)(*(int *)param_3[0x47] + 0x40))();
      AoWSetup_TSetupControl_ListAvailablePlayers_23EDC2EF
                (*(undefined4 *)(local_8 + 0x33c),param_3[0x47]);
      Classes_TStrings_EndUpdate_23EDC2EF(param_3[0x47]);
      local_12 = '\0';
      local_18 = (**(code **)(*(int *)param_3[0x47] + 0x14))();
      while ((local_12 == '\0' && (local_18 != 0))) {
        local_18 = local_18 + -1;
        (**(code **)(*(int *)param_3[0x47] + 0xc))((int *)param_3[0x47],local_18,&local_1c);
        cVar2 = AoWSetup_TSetupControl_PlayerNameToPlayerID_23EDC2EF
                          (*(undefined4 *)(local_8 + 0x33c),local_1c,local_10);
        if (cVar2 != '\0') {
          if ((*(char *)(param_2 + 8) == local_10[0]) && (*(int *)(param_2 + 0xc) == local_c)) {
            local_12 = '\x01';
          }
          else {
            local_12 = '\0';
          }
        }
      }
      if (local_12 == '\0') {
        (**(code **)(*param_3 + 0xb8))(param_3,0xffffffff);
      }
      else {
        (**(code **)(*param_3 + 0xb8))(param_3,local_18);
      }
      (**(code **)(*param_6 + 0x6c))(param_6,0);
    }
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(param_5,0);
    cVar2 = AoWSetup_TSetupControl_GetRemote_23EDC2EF(*(undefined4 *)(local_8 + 0x33c));
    if ((cVar2 == '\0') && ((*(byte *)(param_2 + 0x19) & 1) == 0)) {
      uVar4 = 0;
    }
    else {
      uVar4 = 1;
    }
    (**(code **)(*param_5 + 0x6c))(param_5,uVar4);
    if (*(char *)((int)param_5 + 0x75) != '\0') {
      if ((*(byte *)(param_2 + 0x19) & 1) == 0) {
        cVar2 = AoWSetup_TSetupControl_GetRemote_23EDC2EF(*(undefined4 *)(local_8 + 0x33c));
        if (((cVar2 == '\0') || ((*(byte *)(param_2 + 0x19) & 2) == 0)) ||
           (*(char *)(param_2 + 8) != '\0')) {
          if (*(int *)(param_2 + 0xc) == 0) {
            if (((byte)(*(char *)(param_2 + 8) - 1U) < 3) ||
               ((byte)(*(char *)(param_2 + 8) - 6U) < 2)) {
              AoWE_TranslateRStr_435799AA
                        (*(undefined4 *)
                          (AoWE_TPlayerBehaviorRStr_8BCC1E53_exref +
                          (uint)*(byte *)(param_2 + 0x21) * 4),&local_1c);
              AoWLabel_TAOWLabel_SetGText_23EDC2EF(param_5,local_1c);
            }
          }
          else {
            System_LoadResString_02614E1B(AoWE_TakenRStr_04ECFCC5_exref,&local_20);
            AoWE_TranslateRStr_435799AA(local_20,&local_1c);
            AoWLabel_TAOWLabel_SetGText_23EDC2EF(param_5,local_1c);
          }
        }
        else {
          iVar3 = AoWSetup_TRemotePlayerList_IndexOfRemoteID_23EDC2EF
                            (*(undefined4 *)(*(int *)(*(int *)(local_8 + 0x33c) + 0x28) + 0x38),
                             *(undefined4 *)(param_2 + 0xc));
          if (iVar3 != -1) {
            iVar3 = AoWSetup_TRemotePlayerList_GetPlayers_23EDC2EF
                              (*(undefined4 *)(*(int *)(*(int *)(local_8 + 0x33c) + 0x28) + 0x38),
                               iVar3);
            if (*(char *)(iVar3 + 0x10) == *(char *)(param_2 + 0x18)) {
              System_LoadResString_02614E1B(AoWE_TakenRStr_04ECFCC5_exref,&local_20);
              AoWE_TranslateRStr_435799AA(local_20,&local_1c);
              AoWLabel_TAOWLabel_SetGText_23EDC2EF(param_5,local_1c);
            }
            else {
              System_LoadResString_02614E1B(AoWE_ForcedRStr_04ECFCC5_exref,&local_20);
              AoWE_TranslateRStr_435799AA(local_20,&local_1c);
              AoWLabel_TAOWLabel_SetGText_23EDC2EF(param_5,local_1c);
            }
          }
        }
      }
      else {
        System_LoadResString_02614E1B(AoWE_FixedRStr_04ECFCC5_exref,&local_20);
        AoWE_TranslateRStr_435799AA(local_20,&local_1c);
        AoWLabel_TAOWLabel_SetGText_23EDC2EF(param_5,local_1c);
      }
    }
    cVar2 = AoWSetup_TSetupControl_GetHost_23EDC2EF(*(undefined4 *)(local_8 + 0x33c));
    if ((cVar2 == '\0') ||
       ((2 < (byte)(*(char *)(param_2 + 8) - 1U) && (1 < (byte)(*(char *)(param_2 + 8) - 6U))))) {
      uVar4 = 0;
    }
    else {
      uVar4 = 1;
    }
    (**(code **)(*param_8 + 0x6c))(param_8,uVar4);
    if (*(char *)((int)param_8 + 0x75) != '\0') {
      Classes_TStrings_BeginUpdate_23EDC2EF(param_8[0x47]);
      (**(code **)(*(int *)param_8[0x47] + 0x40))();
      local_11 = '\0';
      pcVar5 = AoWE_TPlayerBehaviorRStr_8BCC1E53_exref;
      do {
        AoWE_TranslateRStr_435799AA(*(undefined4 *)pcVar5,&local_1c);
        (**(code **)(*(int *)param_8[0x47] + 0x38))((int *)param_8[0x47],local_1c,local_11);
        local_11 = local_11 + '\x01';
        pcVar5 = pcVar5 + 4;
      } while (local_11 != '\x06');
      Classes_TStrings_EndUpdate_23EDC2EF(param_8[0x47]);
      (**(code **)(*param_8 + 0xb8))(param_8,*(undefined1 *)(param_2 + 0x21));
    }
    if ((*(byte *)(param_2 + 8) < 4) || ((byte)(*(byte *)(param_2 + 8) - 6) < 2)) {
      uVar4 = 1;
    }
    else {
      uVar4 = 0;
    }
    (**(code **)(*param_7 + 0x6c))(param_7,uVar4);
    if (*(char *)((int)param_7 + 0x75) != '\0') {
      switch(*(undefined1 *)(param_2 + 8)) {
      default:
        AoWImage_TAOWImage_SetCurrent_23EDC2EF(param_7,0);
        break;
      case 1:
        AoWImage_TAOWImage_SetCurrent_23EDC2EF(param_7,1);
        break;
      case 2:
        AoWImage_TAOWImage_SetCurrent_23EDC2EF(param_7,2);
        break;
      case 3:
        AoWImage_TAOWImage_SetCurrent_23EDC2EF(param_7,3);
        break;
      case 6:
        AoWImage_TAOWImage_SetCurrent_23EDC2EF(param_7,4);
        break;
      case 7:
        AoWImage_TAOWImage_SetCurrent_23EDC2EF(param_7,5);
      }
    }
    (**(code **)(*param_4 + 0x6c))(param_4,(*(byte *)(param_2 + 0x19) & 1) != 0);
  }
  puVar1 = puStack_30;
  *in_FS_OFFSET = uStack_38;
  puStack_30 = &LAB_00411457;
  puStack_34 = (undefined1 *)0x41144f;
  System__LStrArrayClr_51F89FF7(&local_20,2,puVar1);
  return;
}

