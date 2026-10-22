
void FUN_004224e4(int param_1)

{
  int *piVar1;
  undefined1 *puVar2;
  undefined1 uVar3;
  undefined4 uVar4;
  uint uVar5;
  int iVar6;
  code *pcVar7;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_24;
  undefined1 *puStack_20;
  undefined1 *puStack_1c;
  undefined4 local_c;
  char local_5;
  
  puStack_1c = &stack0xfffffffc;
  local_c = 0;
  puStack_20 = &LAB_0042277e;
  uStack_24 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_24;
  uVar4 = AoWReg_TAoWRegistry_GetAutoSave_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x5c));
  (**(code **)(**(int **)(param_1 + 100) + 0xac))(*(int **)(param_1 + 100),uVar4);
  uVar4 = AoWReg_TAoWRegistry_GetSkipIntro_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x5c));
  (**(code **)(**(int **)(param_1 + 0x6c) + 0xac))(*(int **)(param_1 + 0x6c),uVar4);
  uVar3 = AoWReg_TAoWRegistry_GetMovementSpeed_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x5c));
  (**(code **)(**(int **)(param_1 + 0xf4) + 0xb8))(*(int **)(param_1 + 0xf4),uVar3);
  (**(code **)(**(int **)(*(int *)(param_1 + 0x88) + 0x11c) + 0x40))();
  local_5 = '\0';
  pcVar7 = AoWE_TCombatResolveModeRStr_05256264_exref;
  do {
    AoWE_TranslateRStr_435799AA(*(undefined4 *)pcVar7,&local_c);
    piVar1 = *(int **)(*(int *)(param_1 + 0x88) + 0x11c);
    (**(code **)(*piVar1 + 0x38))(piVar1,local_c,local_5);
    local_5 = local_5 + '\x01';
    pcVar7 = pcVar7 + 4;
  } while (local_5 != '\x03');
  uVar3 = AoWReg_TAoWRegistry_GetCombatResolveMode_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x5c));
  piVar1 = *(int **)(param_1 + 0x88);
  uVar4 = Classes_TStrings_IndexOfObject_23EDC2EF(piVar1[0x47],uVar3);
  (**(code **)(*piVar1 + 0xb8))(piVar1,uVar4);
  uVar5 = AoWReg_TAoWRegistry_GetClassicWindowPopupEvents_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x5c));
  (**(code **)(**(int **)(param_1 + 0x90) + 0xac))(*(int **)(param_1 + 0x90),(uVar5 & 1) != 0);
  (**(code **)(**(int **)(param_1 + 0x98) + 0xac))(*(int **)(param_1 + 0x98),(uVar5 & 2) != 0);
  (**(code **)(**(int **)(param_1 + 0xa0) + 0xac))(*(int **)(param_1 + 0xa0),(uVar5 & 4) != 0);
  (**(code **)(**(int **)(param_1 + 0xa8) + 0xac))(*(int **)(param_1 + 0xa8),(uVar5 & 8) != 0);
  (**(code **)(**(int **)(param_1 + 0xac) + 0xac))(*(int **)(param_1 + 0xac),(uVar5 & 0x10) != 0);
  (**(code **)(**(int **)(param_1 + 0xb0) + 0xac))(*(int **)(param_1 + 0xb0),(uVar5 & 0x20) != 0);
  (**(code **)(**(int **)(param_1 + 0xe4) + 0xac))(*(int **)(param_1 + 0xe4),(uVar5 & 0x40) != 0);
  (**(code **)(**(int **)(param_1 + 0xfc) + 0xac))(*(int **)(param_1 + 0xfc),(uVar5 & 0x80) != 0);
  uVar5 = AoWReg_TAoWRegistry_GetSimultaneousWindowPopupEvents_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x5c));
  (**(code **)(**(int **)(param_1 + 0xc0) + 0xac))(*(int **)(param_1 + 0xc0),(uVar5 & 1) != 0);
  (**(code **)(**(int **)(param_1 + 0xc4) + 0xac))(*(int **)(param_1 + 0xc4),(uVar5 & 2) != 0);
  (**(code **)(**(int **)(param_1 + 200) + 0xac))(*(int **)(param_1 + 200),(uVar5 & 4) != 0);
  (**(code **)(**(int **)(param_1 + 0xcc) + 0xac))(*(int **)(param_1 + 0xcc),(uVar5 & 8) != 0);
  (**(code **)(**(int **)(param_1 + 0xd0) + 0xac))(*(int **)(param_1 + 0xd0),(uVar5 & 0x10) != 0);
  (**(code **)(**(int **)(param_1 + 0xd4) + 0xac))(*(int **)(param_1 + 0xd4),(uVar5 & 0x20) != 0);
  (**(code **)(**(int **)(param_1 + 0xe8) + 0xac))(*(int **)(param_1 + 0xe8),(uVar5 & 0x40) != 0);
  (**(code **)(**(int **)(param_1 + 0x100) + 0xac))(*(int **)(param_1 + 0x100),(uVar5 & 0x80) != 0);
  iVar6 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                     (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
  AoWEdit_TAOWEdit_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x84),*(undefined4 *)(iVar6 + 0xbc));
  puVar2 = puStack_1c;
  *in_FS_OFFSET = uStack_24;
  puStack_1c = &LAB_00422785;
  puStack_20 = (undefined1 *)0x42277d;
  System__LStrClr_40929B27(&local_c,uStack_24,puVar2);
  return;
}

