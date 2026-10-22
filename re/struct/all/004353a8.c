
void FUN_004353a8(int param_1)

{
  undefined1 *puVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  int iVar5;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_34;
  undefined1 *puStack_30;
  undefined1 *puStack_2c;
  undefined4 local_1c;
  undefined4 local_18;
  int *local_14;
  int local_10;
  char local_9;
  int local_8;
  
  puStack_2c = &stack0xfffffffc;
  local_18 = 0;
  local_1c = 0;
  puStack_30 = &LAB_00435690;
  uStack_34 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_34;
  AoWE_TCombatViewer_Initialize_23EDC2EF(*(undefined4 *)(param_1 + 0x144));
  iVar4 = *(int *)(*(int *)(param_1 + 0x144) + 8);
  *(undefined4 *)(param_1 + 300) = 0;
  iVar5 = 0;
  do {
    switch(iVar5) {
    case 0:
      local_14 = *(int **)(param_1 + 0x78);
      break;
    case 1:
      local_14 = *(int **)(param_1 + 0x84);
      break;
    case 2:
      local_14 = *(int **)(param_1 + 0x90);
      break;
    case 3:
      local_14 = *(int **)(param_1 + 0x9c);
      break;
    case 4:
      local_14 = *(int **)(param_1 + 0xa8);
      break;
    case 5:
      local_14 = *(int **)(param_1 + 0xb4);
      break;
    case 6:
      local_14 = *(int **)(param_1 + 0xc0);
      break;
    case 7:
      local_14 = *(int **)(param_1 + 0x84);
      break;
    default:
      local_14 = (int *)0x0;
    }
    local_10 = AoWE_TCombatPartyList_GetParty_23EDC2EF
                         (*(undefined4 *)(*(int *)(iVar4 + 0xc) + 0x3c),iVar5);
    if (local_10 == 0) {
      if (iVar5 != 7) {
        (**(code **)(*local_14 + 0x6c))(local_14,0);
      }
    }
    else {
      iVar2 = AoWE_TCombatPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)(iVar4 + 0xc) + 0x38),
                         *(undefined1 *)(local_10 + 0x1a));
      local_9 = *(char *)(iVar2 + 8);
      if (local_9 == '\0') {
        iVar2 = 0xc;
      }
      else {
        iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                          (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                           (int)local_9);
        iVar2 = (int)*(char *)(iVar2 + 0xa5);
      }
      AoWImage_TAOWImage_SetCurrent_23EDC2EF(local_14,iVar2 + 0x15);
      (**(code **)(*local_14 + 0x6c))(local_14,1);
    }
    iVar2 = AoWE_TCombatPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(*(int *)(iVar4 + 0xc) + 0x38),
                       *(undefined1 *)(*(int *)(iVar4 + 0xc) + 0x24));
    if (*(char *)(iVar2 + 8) == '\0') {
      System_LoadResString_02614E1B(AoWE_IndependentsRStr_8276FCC3_exref,&local_1c);
      AoWE_TranslateRStr_435799AA(local_1c,&local_18);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x4c),local_18);
    }
    else {
      iVar2 = AoWE_TCombatPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)(iVar4 + 0xc) + 0x38),
                         *(undefined1 *)(*(int *)(iVar4 + 0xc) + 0x24));
      iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                         (int)*(char *)(iVar2 + 8));
      local_8 = (int)*(char *)(iVar2 + 0xa5);
      iVar2 = AoWE_TRaceResourceList_GetRaceResource_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x54),local_8);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF
                (*(undefined4 *)(param_1 + 0x4c),*(undefined4 *)(iVar2 + 0x28));
    }
    iVar2 = AoWE_TCombatPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(*(int *)(iVar4 + 0xc) + 0x38),
                       *(undefined1 *)(*(int *)(iVar4 + 0xc) + 0x25));
    if (*(char *)(iVar2 + 8) == '\0') {
      System_LoadResString_02614E1B(AoWE_IndependentsRStr_8276FCC3_exref,&local_1c);
      AoWE_TranslateRStr_435799AA(local_1c,&local_18);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x54),local_18);
    }
    else {
      iVar2 = AoWE_TCombatPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)(iVar4 + 0xc) + 0x38),
                         *(undefined1 *)(*(int *)(iVar4 + 0xc) + 0x25));
      iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                         (int)*(char *)(iVar2 + 8));
      local_8 = (int)*(char *)(iVar2 + 0xa5);
      iVar2 = AoWE_TRaceResourceList_GetRaceResource_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x54),local_8);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF
                (*(undefined4 *)(param_1 + 0x54),*(undefined4 *)(iVar2 + 0x28));
    }
    iVar5 = iVar5 + 1;
  } while (iVar5 != 8);
  AoWE_TCombatViewer_Play_23EDC2EF(*(undefined4 *)(param_1 + 0x144));
  *(undefined1 *)(*(int *)(param_1 + 0x44) + 0xa4) = 0;
  uVar3 = AoWE_TCombatViewer_GetEndPosition_23EDC2EF(*(undefined4 *)(param_1 + 0x144));
  AoWVScroll_TAOWScrollBar_SetFMax_23EDC2EF(*(undefined4 *)(param_1 + 0xec),uVar3);
  AoWVScroll_TAOWScrollBar_SetFPos_23EDC2EF
            (*(undefined4 *)(param_1 + 0xf0),0x14 - (uint)*(byte *)(*(int *)(param_1 + 0x144) + 0xc)
            );
  iVar4 = AoWE_TCombatViewer_GetEndPosition_23EDC2EF(*(undefined4 *)(param_1 + 0x144));
  if (iVar4 == -1) {
    FUN_004352dc(param_1,3);
  }
  else {
    FUN_004352dc(param_1,0);
  }
  puVar1 = puStack_2c;
  *in_FS_OFFSET = uStack_34;
  puStack_2c = &LAB_00435697;
  puStack_30 = (undefined1 *)0x43568f;
  System__LStrArrayClr_51F89FF7(&local_1c,2,puVar1);
  return;
}

