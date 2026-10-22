
void FUN_00410bd8(int param_1,undefined4 param_2)

{
  int *piVar1;
  int iVar2;
  undefined1 *puVar3;
  undefined1 uVar4;
  char cVar5;
  uint uVar6;
  int iVar7;
  code *pcVar8;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_28;
  undefined1 *puStack_24;
  undefined1 *puStack_20;
  undefined4 local_10;
  undefined4 local_c;
  undefined3 uStack_8;
  char local_5;
  
  puStack_20 = &stack0xfffffffc;
  uStack_8 = 0;
  local_5 = 0;
  local_c = 0;
  local_10 = 0;
  puStack_24 = &LAB_00410ee7;
  uStack_28 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_28;
  *(undefined4 *)(param_1 + 0x33c) = param_2;
  (**(code **)(**(int **)(*(int *)PTR_DAT_0045a28c + 0x44) + 0x6c))
            (*(int **)(*(int *)PTR_DAT_0045a28c + 0x44),1);
  (**(code **)(**(int **)(param_1 + 0xa0) + 0x6c))(*(int **)(param_1 + 0xa0),1);
  (**(code **)(**(int **)(*(int *)(param_1 + 0x44) + 0x118) + 0x40))();
  AoWEdit_TAOWEdit_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x50),0);
  *(undefined4 *)(param_1 + 0x338) = 100;
  *(undefined4 *)(param_1 + 0x340) = 0xf;
  uVar6 = AoWSetup_TSetupControl_GetRemote_23EDC2EF(*(undefined4 *)(param_1 + 0x33c));
  (**(code **)(**(int **)(param_1 + 0x30c) + 0x6c))(*(int **)(param_1 + 0x30c),uVar6 ^ 1);
  uVar4 = AoWSetup_TSetupControl_GetRemote_23EDC2EF(*(undefined4 *)(param_1 + 0x33c));
  *(undefined1 *)(*(int *)(param_1 + 0x44) + 0xc0) = uVar4;
  uVar4 = AoWSetup_TSetupControl_GetRemote_23EDC2EF(*(undefined4 *)(param_1 + 0x33c));
  *(undefined1 *)(*(int *)(param_1 + 0x50) + 0xc0) = uVar4;
  cVar5 = AoWSetup_TSetupControl_GetHost_23EDC2EF(*(undefined4 *)(param_1 + 0x33c));
  if ((cVar5 != '\0') && (*(int *)(*(int *)(*(int *)(param_1 + 0x33c) + 0x28) + 0x14) == 0)) {
    FUN_0041243c(param_1);
  }
  cVar5 = AoWSetup_TSetupControl_GetRemote_23EDC2EF(*(undefined4 *)(param_1 + 0x33c));
  if (cVar5 == '\0') {
    (**(code **)(**(int **)(param_1 + 0x328) + 0x6c))(*(int **)(param_1 + 0x328),0);
    (**(code **)(**(int **)(param_1 + 0x32c) + 0x6c))(*(int **)(param_1 + 0x32c),0);
  }
  else {
    (**(code **)(**(int **)(param_1 + 0x328) + 0x6c))(*(int **)(param_1 + 0x328),1);
    (**(code **)(**(int **)(param_1 + 0x32c) + 0x6c))(*(int **)(param_1 + 0x32c),1);
    (**(code **)(**(int **)(*(int *)(param_1 + 0x328) + 0x11c) + 0x40))();
    local_5 = '\0';
    pcVar8 = AoWE_TTacticalCombatAvailabilityRStr_F89459F7_exref;
    do {
      AoWE_TranslateRStr_435799AA(*(undefined4 *)pcVar8,&local_c);
      piVar1 = *(int **)(*(int *)(param_1 + 0x328) + 0x11c);
      (**(code **)(*piVar1 + 0x38))(piVar1,local_c,local_5);
      local_5 = local_5 + '\x01';
      pcVar8 = pcVar8 + 4;
    } while (local_5 != '\x03');
    local_5 = 3;
  }
  if (*(char *)(*(int *)(*(int *)(param_1 + 0x33c) + 0x28) + 0x3c) == '\x02') {
    (**(code **)(**(int **)(param_1 + 0x28c) + 0x70))(*(int **)(param_1 + 0x28c),0x808080);
    *(undefined1 *)(*(int *)(param_1 + 0x288) + 0xc0) = 0;
    (**(code **)(**(int **)(*(int *)(param_1 + 0xd8) + 0x11c) + 0x40))();
    System_LoadResString_02614E1B(AoWE_ClassicRStr_04ECFCC5_exref,&local_10);
    AoWE_TranslateRStr_435799AA(local_10,&local_c);
    piVar1 = *(int **)(*(int *)(param_1 + 0xd8) + 0x11c);
    (**(code **)(*piVar1 + 0x38))(piVar1,local_c,1);
  }
  else {
    (**(code **)(**(int **)(param_1 + 0x28c) + 0x70))
              (*(int **)(param_1 + 0x28c),*(undefined4 *)(*(int *)(param_1 + 0x31c) + 0x24));
    *(undefined1 *)(*(int *)(param_1 + 0x288) + 0xc0) = 1;
    (**(code **)(**(int **)(*(int *)(param_1 + 0xd8) + 0x11c) + 0x40))();
    System_LoadResString_02614E1B(AoWE_SimultaneousRStr_04ECFCC5_exref,&local_10);
    AoWE_TranslateRStr_435799AA(local_10,&local_c);
    piVar1 = *(int **)(*(int *)(param_1 + 0xd8) + 0x11c);
    (**(code **)(*piVar1 + 0x38))(piVar1,local_c,2);
    System_LoadResString_02614E1B(AoWE_ClassicRStr_04ECFCC5_exref,&local_10);
    AoWE_TranslateRStr_435799AA(local_10,&local_c);
    piVar1 = *(int **)(*(int *)(param_1 + 0xd8) + 0x11c);
    (**(code **)(*piVar1 + 0x38))(piVar1,local_c,1);
  }
  iVar2 = *(int *)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0x14);
  if (iVar2 != -1) {
    iVar7 = (**(code **)(**(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0xc) + 0x14))();
    uVar4 = iVar2 == iVar7;
    if (iVar2 < iVar7) {
      (**(code **)(**(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0xc) + 0xc))
                (*(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0xc),
                 *(undefined4 *)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0x14),&local_c);
      System__LStrCmp_51F89FF7(local_c,"SONGS\\TITLE.IT");
      if ((bool)uVar4) goto LAB_00410ec5;
    }
  }
  (**(code **)(**(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0xc) + 0x40))();
  (**(code **)(**(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0xc) + 0x34))
            (*(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0xc),"SONGS\\TITLE.IT");
  Sound_TSongPlayer_Play_23EDC2EF(*(undefined4 *)Sound_SongPlayer_F3DCC77F_exref,0);
LAB_00410ec5:
  FUN_00411460(param_1);
  puVar3 = puStack_20;
  *in_FS_OFFSET = uStack_28;
  puStack_20 = &LAB_00410eee;
  puStack_24 = (undefined1 *)0x410ee6;
  System__LStrArrayClr_51F89FF7(&local_10,2,puVar3);
  return;
}

