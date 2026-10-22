
void FUN_00411460(int param_1)

{
  int *piVar1;
  undefined1 *puVar2;
  char cVar3;
  int iVar4;
  undefined4 uVar5;
  uint uVar6;
  int iVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  int iVar10;
  undefined4 *in_FS_OFFSET;
  undefined4 *puVar11;
  undefined4 uVar12;
  undefined4 uVar13;
  undefined4 uVar14;
  undefined4 uVar15;
  undefined4 uStack_50;
  undefined1 *puStack_4c;
  undefined1 *puStack_48;
  undefined4 local_38;
  undefined1 local_34;
  undefined4 local_30;
  undefined1 local_2c;
  undefined4 local_28;
  undefined1 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  int local_18;
  int local_14;
  undefined4 local_10;
  undefined4 local_c;
  int local_8;
  
  puStack_48 = &stack0xfffffffc;
  local_1c = 0;
  local_20 = 0;
  local_c = 0;
  local_10 = 0;
  puStack_4c = &LAB_00411f33;
  uStack_50 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_50;
  iVar10 = *(int *)(param_1 + 0x33c);
  puVar2 = &stack0xfffffffc;
  if (iVar10 == 0) goto LAB_00411f0b;
  (**(code **)(**(int **)(*(int *)(param_1 + 0x5c) + 0x114) + 0x40))();
  (**(code **)(**(int **)(*(int *)(param_1 + 0x2e0) + 0x114) + 0x40))();
  (**(code **)(**(int **)(*(int *)(param_1 + 0x2d8) + 0x114) + 0x40))();
  iVar4 = (**(code **)(**(int **)(*(int *)(iVar10 + 0x28) + 0x38) + 0x54))();
  if (-1 < iVar4 + -1) {
    local_8 = 0;
    local_18 = iVar4;
    do {
      local_14 = AoWSetup_TRemotePlayerList_GetPlayers_23EDC2EF
                           (*(undefined4 *)(*(int *)(iVar10 + 0x28) + 0x38),local_8);
      piVar1 = *(int **)(*(int *)(param_1 + 0x5c) + 0x114);
      (**(code **)(*piVar1 + 0x38))
                (piVar1,*(undefined4 *)(local_14 + 8),*(undefined4 *)(local_14 + 0xc));
      if (*(char *)(local_14 + 0x10) == -1) {
        System_LoadResString_02614E1B(AoWE_NoneRStr_8276FCC3_exref,&local_20);
        AoWE_TranslateRStr_435799AA(local_20,&local_1c);
        piVar1 = *(int **)(*(int *)(param_1 + 0x2d8) + 0x114);
        (**(code **)(*piVar1 + 0x34))(piVar1,local_1c);
      }
      else {
        AoWE_TranslateRStr_435799AA
                  (*(undefined4 *)
                    (AoWE_RaceNamesPluralRStr_B11890CE_exref + *(char *)(local_14 + 0x10) * 4),
                   &local_1c);
        piVar1 = *(int **)(*(int *)(param_1 + 0x2d8) + 0x114);
        (**(code **)(*piVar1 + 0x34))(piVar1,local_1c);
      }
      piVar1 = *(int **)(*(int *)(param_1 + 0x2e0) + 0x114);
      (**(code **)(*piVar1 + 0x38))(piVar1,0,*(undefined4 *)(local_14 + 0xc));
      local_8 = local_8 + 1;
      local_18 = local_18 + -1;
    } while (local_18 != 0);
  }
  iVar4 = (**(code **)(**(int **)(*(int *)(param_1 + 0x5c) + 0x114) + 0x14))();
  AoWVScroll_TAOWScrollBar_SetFMax_23EDC2EF(*(undefined4 *)(param_1 + 0x2e4),iVar4 + -1);
  FUN_00412624(param_1,*(undefined4 *)(*(int *)(param_1 + 0x5c) + 0x120));
  (**(code **)(**(int **)(param_1 + 0x288) + 0xac))
            (*(int **)(param_1 + 0x288),
             *(undefined1 *)(*(int *)(*(int *)(param_1 + 0x33c) + 0x28) + 0x2a));
  piVar1 = *(int **)(param_1 + 0xd8);
  uVar5 = Classes_TStrings_IndexOfObject_23EDC2EF
                    (piVar1[0x47],*(undefined1 *)(*(int *)(*(int *)(param_1 + 0x33c) + 0x28) + 0x24)
                    );
  (**(code **)(*piVar1 + 0xb8))(piVar1,uVar5);
  (**(code **)(**(int **)(param_1 + 0x2f0) + 0xac))
            (*(int **)(param_1 + 0x2f0),
             *(undefined1 *)(*(int *)(*(int *)(param_1 + 0x33c) + 0x28) + 0x25));
  AoWVScroll_TAOWScrollBar_SetFPos_23EDC2EF
            (*(undefined4 *)(param_1 + 0xc4),
             *(undefined4 *)(*(int *)(*(int *)(param_1 + 0x33c) + 0x28) + 0x34));
  iVar4 = *(int *)(*(int *)(*(int *)(param_1 + 0x33c) + 0x28) + 0x30);
  if (iVar4 == 0) {
    AoWVScroll_TAOWScrollBar_SetFPos_23EDC2EF(*(undefined4 *)(param_1 + 0xd0),0);
  }
  else {
    AoWVScroll_TAOWScrollBar_SetFPos_23EDC2EF(*(undefined4 *)(param_1 + 0xd0),iVar4 / 10);
  }
  FUN_004128b4(param_1);
  FUN_00412778(param_1);
  cVar3 = AoWSetup_TSetupControl_GetHost_23EDC2EF(*(undefined4 *)(param_1 + 0x33c));
  if (cVar3 == '\0') {
LAB_0041169b:
    uVar5 = 0;
  }
  else {
    cVar3 = AoWSetup_TSetupControl_GetPlayerSetupValid_23EDC2EF(*(undefined4 *)(param_1 + 0x33c));
    if (cVar3 == '\0') goto LAB_0041169b;
    uVar5 = 1;
  }
  (**(code **)(**(int **)(param_1 + 0x98) + 0x6c))(*(int **)(param_1 + 0x98),uVar5);
  uVar5 = AoWSetup_TSetupControl_GetHost_23EDC2EF(*(undefined4 *)(param_1 + 0x33c));
  (**(code **)(**(int **)(param_1 + 0x94) + 0x6c))(*(int **)(param_1 + 0x94),uVar5);
  uVar6 = AoWSetup_TSetupControl_GetHost_23EDC2EF(*(undefined4 *)(param_1 + 0x33c));
  (**(code **)(**(int **)(param_1 + 0x2e8) + 0x6c))(*(int **)(param_1 + 0x2e8),uVar6 ^ 1);
  if ((*(char *)(*(int *)(param_1 + 0x2e8) + 0x75) == '\0') &&
     (*(int *)(*(int *)(*(int *)(param_1 + 0x33c) + 0x28) + 0x20) != 0)) {
    uVar5 = 1;
  }
  else {
    uVar5 = 0;
  }
  (**(code **)(**(int **)(param_1 + 0x308) + 0x6c))(*(int **)(param_1 + 0x308),uVar5);
  (**(code **)(**(int **)(param_1 + 0x310) + 0xac))
            (*(int **)(param_1 + 0x310),
             *(undefined1 *)(*(int *)(*(int *)(param_1 + 0x33c) + 0x28) + 0x26));
  (**(code **)(**(int **)(param_1 + 0x318) + 0xac))
            (*(int **)(param_1 + 0x318),
             *(undefined1 *)(*(int *)(*(int *)(param_1 + 0x33c) + 0x28) + 0x27));
  piVar1 = *(int **)(param_1 + 0x328);
  uVar5 = Classes_TStrings_IndexOfObject_23EDC2EF
                    (piVar1[0x47],*(undefined1 *)(*(int *)(*(int *)(param_1 + 0x33c) + 0x28) + 0x3d)
                    );
  (**(code **)(*piVar1 + 0xb8))(piVar1,uVar5);
  if ((*(char *)(*(int *)(param_1 + 0x328) + 0x75) == '\0') ||
     (*(char *)(*(int *)(*(int *)(param_1 + 0x33c) + 0x28) + 0x3d) == '\0')) {
    uVar5 = 0;
  }
  else {
    uVar5 = 1;
  }
  (**(code **)(**(int **)(param_1 + 0x330) + 0x6c))(*(int **)(param_1 + 0x330),uVar5);
  (**(code **)(**(int **)(param_1 + 0x330) + 0xac))
            (*(int **)(param_1 + 0x330),
             *(undefined1 *)(*(int *)(*(int *)(param_1 + 0x33c) + 0x28) + 0x3e));
  (**(code **)(**(int **)(param_1 + 0x334) + 0x6c))
            (*(int **)(param_1 + 0x334),*(undefined1 *)(*(int *)(param_1 + 0x330) + 0x75));
  uVar5 = AoWSetup_TSetupControl_GetRemote_23EDC2EF(*(undefined4 *)(param_1 + 0x33c));
  (**(code **)(**(int **)(param_1 + 0x280) + 0x6c))(*(int **)(param_1 + 0x280),uVar5);
  uVar5 = AoWSetup_TSetupControl_GetRemote_23EDC2EF(*(undefined4 *)(param_1 + 0x33c));
  (**(code **)(**(int **)(param_1 + 0x278) + 0x6c))(*(int **)(param_1 + 0x278),uVar5);
  if (*(char *)(*(int *)(param_1 + 0x278) + 0x75) != '\0') {
    (**(code **)(**(int **)(*(int *)(param_1 + 0x278) + 0x11c) + 0x40))();
    System_LoadResString_02614E1B(AoWE_NoneRStr_8276FCC3_exref,&local_20);
    AoWE_TranslateRStr_435799AA(local_20,&local_1c);
    piVar1 = *(int **)(*(int *)(param_1 + 0x278) + 0x11c);
    (**(code **)(*piVar1 + 0x38))(piVar1,local_1c,0xffffffff);
    iVar4 = (**(code **)(**(int **)(*(int *)(param_1 + 0x33c) + 0x28) + 0x54))();
    if (-1 < iVar4 + -1) {
      local_8 = 0;
      local_18 = iVar4;
      do {
        iVar4 = AoWSetup_TSetupSettings_GetSettings_23EDC2EF
                          (*(undefined4 *)(*(int *)(param_1 + 0x33c) + 0x28),local_8);
        if ((*(byte *)(iVar4 + 0x19) & 1) == 0) {
          iVar4 = AoWSetup_TSetupSettings_GetSettings_23EDC2EF
                            (*(undefined4 *)(*(int *)(param_1 + 0x33c) + 0x28),local_8);
          iVar4 = (int)*(char *)(iVar4 + 0x18);
          iVar7 = AoWSetup_TSetupSettings_GetSettings_23EDC2EF
                            (*(undefined4 *)(*(int *)(param_1 + 0x33c) + 0x28),local_8);
          AoWE_TranslateRStr_435799AA
                    (*(undefined4 *)
                      (AoWE_RaceNamesPluralRStr_B11890CE_exref + *(char *)(iVar7 + 0x18) * 4),
                     &local_1c);
          piVar1 = *(int **)(*(int *)(param_1 + 0x278) + 0x11c);
          (**(code **)(*piVar1 + 0x38))(piVar1,local_1c,iVar4);
        }
        local_8 = local_8 + 1;
        local_18 = local_18 + -1;
      } while (local_18 != 0);
    }
    piVar1 = *(int **)(param_1 + 0x278);
    piVar1[0x4a] = 5;
    *(undefined1 *)((int)piVar1 + 0x2e) = 0;
    uVar5 = Classes_TStrings_IndexOfObject_23EDC2EF
                      (piVar1[0x47],
                       (int)*(char *)(*(int *)(*(int *)(param_1 + 0x33c) + 0x34) + 0x10));
    (**(code **)(*piVar1 + 0xb8))(piVar1,uVar5);
  }
  uVar5 = *(undefined4 *)(param_1 + 0xe4);
  uVar15 = *(undefined4 *)(param_1 + 0xa4);
  uVar14 = *(undefined4 *)(param_1 + 0x29c);
  uVar13 = *(undefined4 *)(param_1 + 0xe0);
  uVar12 = *(undefined4 *)(param_1 + 0x298);
  uVar9 = *(undefined4 *)(iVar10 + 0x28);
  uVar8 = AoWSetup_TSetupSettings_IndexOfRace_23EDC2EF(uVar9,0);
  uVar9 = AoWSetup_TSetupSettings_GetSettings_23EDC2EF(uVar9,uVar8);
  FUN_00410f2c(param_1,uVar9,*(undefined4 *)(param_1 + 0xac),uVar12,uVar13,uVar14,uVar15,uVar5);
  uVar5 = *(undefined4 *)(param_1 + 0x11c);
  uVar15 = *(undefined4 *)(param_1 + 0x124);
  uVar14 = *(undefined4 *)(param_1 + 0x134);
  uVar13 = *(undefined4 *)(param_1 + 0x128);
  uVar12 = *(undefined4 *)(param_1 + 0x2a0);
  uVar9 = *(undefined4 *)(iVar10 + 0x28);
  uVar8 = AoWSetup_TSetupSettings_IndexOfRace_23EDC2EF(uVar9,1);
  uVar9 = AoWSetup_TSetupSettings_GetSettings_23EDC2EF(uVar9,uVar8);
  FUN_00410f2c(param_1,uVar9,*(undefined4 *)(param_1 + 300),uVar12,uVar13,uVar14,uVar15,uVar5);
  uVar5 = *(undefined4 *)(param_1 + 0xf8);
  uVar15 = *(undefined4 *)(param_1 + 0x100);
  uVar14 = *(undefined4 *)(param_1 + 0x104);
  uVar13 = *(undefined4 *)(param_1 + 0x110);
  uVar12 = *(undefined4 *)(param_1 + 0x2a4);
  uVar9 = *(undefined4 *)(iVar10 + 0x28);
  uVar8 = AoWSetup_TSetupSettings_IndexOfRace_23EDC2EF(uVar9,2);
  uVar9 = AoWSetup_TSetupSettings_GetSettings_23EDC2EF(uVar9,uVar8);
  FUN_00410f2c(param_1,uVar9,*(undefined4 *)(param_1 + 0x108),uVar12,uVar13,uVar14,uVar15,uVar5);
  uVar5 = *(undefined4 *)(param_1 + 0x294);
  uVar15 = *(undefined4 *)(param_1 + 0x154);
  uVar14 = *(undefined4 *)(param_1 + 0x13c);
  uVar13 = *(undefined4 *)(param_1 + 0x138);
  uVar12 = *(undefined4 *)(param_1 + 0x2a8);
  uVar9 = *(undefined4 *)(iVar10 + 0x28);
  uVar8 = AoWSetup_TSetupSettings_IndexOfRace_23EDC2EF(uVar9,3);
  uVar9 = AoWSetup_TSetupSettings_GetSettings_23EDC2EF(uVar9,uVar8);
  FUN_00410f2c(param_1,uVar9,*(undefined4 *)(param_1 + 0x14c),uVar12,uVar13,uVar14,uVar15,uVar5);
  uVar5 = *(undefined4 *)(param_1 + 0x168);
  uVar15 = *(undefined4 *)(param_1 + 0x158);
  uVar14 = *(undefined4 *)(param_1 + 0x178);
  uVar13 = *(undefined4 *)(param_1 + 0x174);
  uVar12 = *(undefined4 *)(param_1 + 0x2b8);
  uVar9 = *(undefined4 *)(iVar10 + 0x28);
  uVar8 = AoWSetup_TSetupSettings_IndexOfRace_23EDC2EF(uVar9,4);
  uVar9 = AoWSetup_TSetupSettings_GetSettings_23EDC2EF(uVar9,uVar8);
  FUN_00410f2c(param_1,uVar9,*(undefined4 *)(param_1 + 0x160),uVar12,uVar13,uVar14,uVar15,uVar5);
  uVar5 = *(undefined4 *)(param_1 + 0x184);
  uVar15 = *(undefined4 *)(param_1 + 0x18c);
  uVar14 = *(undefined4 *)(param_1 + 400);
  uVar13 = *(undefined4 *)(param_1 + 0x19c);
  uVar12 = *(undefined4 *)(param_1 + 0x2b4);
  uVar9 = *(undefined4 *)(iVar10 + 0x28);
  uVar8 = AoWSetup_TSetupSettings_IndexOfRace_23EDC2EF(uVar9,5);
  uVar9 = AoWSetup_TSetupSettings_GetSettings_23EDC2EF(uVar9,uVar8);
  FUN_00410f2c(param_1,uVar9,*(undefined4 *)(param_1 + 0x194),uVar12,uVar13,uVar14,uVar15,uVar5);
  uVar5 = *(undefined4 *)(param_1 + 0x1a8);
  uVar15 = *(undefined4 *)(param_1 + 0x1b0);
  uVar14 = *(undefined4 *)(param_1 + 0x1b4);
  uVar13 = *(undefined4 *)(param_1 + 0x1c0);
  uVar12 = *(undefined4 *)(param_1 + 0x2b0);
  uVar9 = *(undefined4 *)(iVar10 + 0x28);
  uVar8 = AoWSetup_TSetupSettings_IndexOfRace_23EDC2EF(uVar9,6);
  uVar9 = AoWSetup_TSetupSettings_GetSettings_23EDC2EF(uVar9,uVar8);
  FUN_00410f2c(param_1,uVar9,*(undefined4 *)(param_1 + 0x1b8),uVar12,uVar13,uVar14,uVar15,uVar5);
  uVar5 = *(undefined4 *)(param_1 + 0x1d4);
  uVar15 = *(undefined4 *)(param_1 + 0x1e4);
  uVar14 = *(undefined4 *)(param_1 + 0x1c8);
  uVar13 = *(undefined4 *)(param_1 + 0x1c4);
  uVar12 = *(undefined4 *)(param_1 + 0x2ac);
  uVar9 = *(undefined4 *)(iVar10 + 0x28);
  uVar8 = AoWSetup_TSetupSettings_IndexOfRace_23EDC2EF(uVar9,7);
  uVar9 = AoWSetup_TSetupSettings_GetSettings_23EDC2EF(uVar9,uVar8);
  FUN_00410f2c(param_1,uVar9,*(undefined4 *)(param_1 + 0x1dc),uVar12,uVar13,uVar14,uVar15,uVar5);
  uVar5 = *(undefined4 *)(param_1 + 0x1f8);
  uVar15 = *(undefined4 *)(param_1 + 0x1e8);
  uVar14 = *(undefined4 *)(param_1 + 0x208);
  uVar13 = *(undefined4 *)(param_1 + 0x204);
  uVar12 = *(undefined4 *)(param_1 + 0x2c8);
  uVar9 = *(undefined4 *)(iVar10 + 0x28);
  uVar8 = AoWSetup_TSetupSettings_IndexOfRace_23EDC2EF(uVar9,8);
  uVar9 = AoWSetup_TSetupSettings_GetSettings_23EDC2EF(uVar9,uVar8);
  FUN_00410f2c(param_1,uVar9,*(undefined4 *)(param_1 + 0x1f0),uVar12,uVar13,uVar14,uVar15,uVar5);
  uVar5 = *(undefined4 *)(param_1 + 0x214);
  uVar15 = *(undefined4 *)(param_1 + 0x21c);
  uVar14 = *(undefined4 *)(param_1 + 0x220);
  uVar13 = *(undefined4 *)(param_1 + 0x22c);
  uVar12 = *(undefined4 *)(param_1 + 0x2c4);
  uVar9 = *(undefined4 *)(iVar10 + 0x28);
  uVar8 = AoWSetup_TSetupSettings_IndexOfRace_23EDC2EF(uVar9,9);
  uVar9 = AoWSetup_TSetupSettings_GetSettings_23EDC2EF(uVar9,uVar8);
  FUN_00410f2c(param_1,uVar9,*(undefined4 *)(param_1 + 0x224),uVar12,uVar13,uVar14,uVar15,uVar5);
  uVar5 = *(undefined4 *)(param_1 + 0x238);
  uVar15 = *(undefined4 *)(param_1 + 0x240);
  uVar14 = *(undefined4 *)(param_1 + 0x244);
  uVar13 = *(undefined4 *)(param_1 + 0x250);
  uVar12 = *(undefined4 *)(param_1 + 0x2c0);
  uVar9 = *(undefined4 *)(iVar10 + 0x28);
  uVar8 = AoWSetup_TSetupSettings_IndexOfRace_23EDC2EF(uVar9,10);
  uVar9 = AoWSetup_TSetupSettings_GetSettings_23EDC2EF(uVar9,uVar8);
  FUN_00410f2c(param_1,uVar9,*(undefined4 *)(param_1 + 0x248),uVar12,uVar13,uVar14,uVar15,uVar5);
  uVar5 = *(undefined4 *)(param_1 + 0x264);
  uVar15 = *(undefined4 *)(param_1 + 0x274);
  uVar14 = *(undefined4 *)(param_1 + 600);
  uVar13 = *(undefined4 *)(param_1 + 0x254);
  uVar12 = *(undefined4 *)(param_1 + 700);
  uVar9 = *(undefined4 *)(iVar10 + 0x28);
  uVar8 = AoWSetup_TSetupSettings_IndexOfRace_23EDC2EF(uVar9,0xb);
  uVar9 = AoWSetup_TSetupSettings_GetSettings_23EDC2EF(uVar9,uVar8);
  FUN_00410f2c(param_1,uVar9,*(undefined4 *)(param_1 + 0x26c),uVar12,uVar13,uVar14,uVar15,uVar5);
  cVar3 = AoWSetup_TSetupControl_GetRemote_23EDC2EF(*(undefined4 *)(param_1 + 0x33c));
  if (cVar3 == '\0') {
    (**(code **)(**(int **)(param_1 + 0x2f8) + 0x6c))(*(int **)(param_1 + 0x2f8),0);
    (**(code **)(**(int **)(param_1 + 0x300) + 0x6c))(*(int **)(param_1 + 0x300),0);
    (**(code **)(**(int **)(param_1 + 0x2fc) + 0x6c))(*(int **)(param_1 + 0x2fc),0);
  }
  else {
    cVar3 = *(char *)(*(int *)(*(int *)PTR_DAT_0045a454 + 0x44) + 0x21c);
    if (cVar3 == '\x02') {
      System_LoadResString_02614E1B(AoWE_TCPIPRStr_04ECFCC5_exref,&local_1c);
      AoWE_TranslateRStr_435799AA(local_1c,&local_10);
    }
    else if (cVar3 == '\x03') {
      System_LoadResString_02614E1B(AoWE_IPXRStr_04ECFCC5_exref,&local_1c);
      AoWE_TranslateRStr_435799AA(local_1c,&local_10);
    }
    else if (cVar3 == '\x04') {
      System_LoadResString_02614E1B(AoWE_ModemRStr_04ECFCC5_exref,&local_1c);
      AoWE_TranslateRStr_435799AA(local_1c,&local_10);
    }
    else if (cVar3 == '\x05') {
      System_LoadResString_02614E1B(AoWE_SerialRStr_04ECFCC5_exref,&local_1c);
      AoWE_TranslateRStr_435799AA(local_1c,&local_10);
    }
    else {
      System__LStrClr_40929B27(&local_10);
    }
    iVar10 = *(int *)(*(int *)(param_1 + 0x33c) + 0x28);
    iVar10 = AoWSetup_TRemotePlayerList_IndexOfRemoteID_23EDC2EF
                       (*(undefined4 *)(iVar10 + 0x38),*(undefined4 *)(iVar10 + 0xc));
    if (iVar10 == -1) {
      System__LStrLAsg_51F89FF7(&local_c,&DAT_00411f4c);
    }
    else {
      iVar10 = AoWSetup_TRemotePlayerList_GetPlayers_23EDC2EF
                         (*(undefined4 *)(*(int *)(*(int *)(param_1 + 0x33c) + 0x28) + 0x38),iVar10)
      ;
      System__LStrLAsg_51F89FF7(&local_c,*(undefined4 *)(iVar10 + 8));
    }
    (**(code **)(**(int **)(param_1 + 0x2f8) + 0x6c))(*(int **)(param_1 + 0x2f8),1);
    (**(code **)(**(int **)(param_1 + 0x300) + 0x6c))(*(int **)(param_1 + 0x300),1);
    (**(code **)(**(int **)(param_1 + 0x2fc) + 0x6c))(*(int **)(param_1 + 0x2fc),1);
    puVar11 = &local_1c;
    System_LoadResString_02614E1B(AoWE_XHostYRStr_04ECFCC5_exref,&local_1c);
    AoWE_TranslateRStr_435799AA(local_1c,&local_20);
    local_30 = local_10;
    local_2c = 0xb;
    local_28 = local_c;
    local_24 = 0xb;
    AoWE_Format_D8F48273(local_20,&local_30,1,puVar11);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x2f8),local_1c);
    puVar11 = &local_1c;
    System_LoadResString_02614E1B(AoWE_PlayerNameXRStr_04ECFCC5_exref,&local_1c);
    AoWE_TranslateRStr_435799AA(local_1c,&local_20);
    local_38 = *(undefined4 *)(*(int *)(*(int *)(param_1 + 0x33c) + 0x34) + 8);
    local_34 = 0xb;
    AoWE_Format_D8F48273(local_20,&local_38,0,puVar11);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x2fc),local_1c);
    puVar11 = &local_1c;
    System_LoadResString_02614E1B(AoWE_FileNameXRStr_04ECFCC5_exref,&local_1c);
    AoWE_TranslateRStr_435799AA(local_1c,&local_20);
    local_38 = *(undefined4 *)(*(int *)(*(int *)(param_1 + 0x33c) + 0x28) + 0x14);
    local_34 = 0xb;
    AoWE_Format_D8F48273(local_20,&local_38,0,puVar11);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x300),local_1c);
    if (*(char *)(*(int *)(*(int *)(param_1 + 0x33c) + 0x34) + 0x11) == '\x01') {
      (**(code **)(**(int **)(param_1 + 0x300) + 0x70))(*(int **)(param_1 + 0x300),0xff);
    }
    else {
      (**(code **)(**(int **)(param_1 + 0x300) + 0x70))(*(int **)(param_1 + 0x300),0x9d9d9d);
    }
  }
  puVar11 = &local_1c;
  System_LoadResString_02614E1B(AoWE_ScenarioXRStr_04ECFCC5_exref,&local_1c);
  AoWE_TranslateRStr_435799AA(local_1c,&local_20);
  local_38 = *(undefined4 *)(*(int *)(*(int *)(param_1 + 0x33c) + 0x28) + 0x10);
  local_34 = 0xb;
  AoWE_Format_D8F48273(local_20,&local_38,0,puVar11);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x2f4),local_1c);
  *(undefined1 *)(*(int *)(*(int *)PTR_DAT_0045a28c + 0x44) + 0xa4) = 0;
  puVar2 = puStack_48;
LAB_00411f0b:
  puStack_48 = puVar2;
  puVar2 = puStack_48;
  *in_FS_OFFSET = uStack_50;
  puStack_48 = &LAB_00411f3a;
  puStack_4c = (undefined1 *)0x411f25;
  System__LStrArrayClr_51F89FF7(&local_20,2,puVar2);
  puStack_4c = (undefined1 *)0x411f32;
  System__LStrArrayClr_51F89FF7(&local_10,2);
  return;
}

