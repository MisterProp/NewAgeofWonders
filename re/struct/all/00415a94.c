
void FUN_00415a94(int param_1)

{
  int *piVar1;
  undefined1 *puVar2;
  char cVar3;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_8;
  
  puStack_14 = &stack0xfffffffc;
  local_8 = 0;
  puStack_18 = &LAB_00415c31;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  piVar1 = *(int **)(*(int *)(param_1 + 0x2d8) + 0x14);
  (**(code **)(*piVar1 + 0x3c))
            (piVar1,*(undefined4 *)(param_1 + 0x2e4),*(undefined4 *)AoWE_AoWEngine_7E705837_exref);
  piVar1 = *(int **)(*(int *)(param_1 + 0x2d8) + 0x14);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x158),piVar1[0x18]);
  cVar3 = (**(code **)(*piVar1 + 0xa8))();
  SysUtils_IntToStr_0F6FDFF6((int)cVar3,&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 300),local_8);
  cVar3 = (**(code **)(*piVar1 + 0xac))();
  SysUtils_IntToStr_0F6FDFF6((int)cVar3,&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x130),local_8);
  cVar3 = (**(code **)(*piVar1 + 0xb0))();
  SysUtils_IntToStr_0F6FDFF6((int)cVar3,&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x138),local_8);
  cVar3 = (**(code **)(*piVar1 + 0xb4))();
  SysUtils_IntToStr_0F6FDFF6((int)cVar3,&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x13c),local_8);
  cVar3 = (**(code **)(*piVar1 + 0xd0))();
  SysUtils_IntToStr_0F6FDFF6((int)cVar3,&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x140),local_8);
  cVar3 = (**(code **)(*piVar1 + 0xd4))();
  SysUtils_IntToStr_0F6FDFF6((int)cVar3,&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x134),local_8);
  (**(code **)(**(int **)(*(int *)(param_1 + 0x148) + 0x114) + 0x40))();
  AoWE_TAbilityOwner_ListAbilities_23EDC2EF
            (piVar1,*(undefined4 *)(*(int *)(param_1 + 0x148) + 0x114));
  AoWE_TranslateRStr_435799AA
            (*(undefined4 *)(AoWE_RaceNamesPluralRStr_B11890CE_exref + (char)piVar1[0x1a] * 4),
             &local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x234),local_8);
  AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(param_1 + 0x15c),(char)piVar1[0x1a] + 0xd);
  AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(param_1 + 0x160),(char)piVar1[0x1a] + 0xd);
  puVar2 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_00415c38;
  puStack_18 = (undefined1 *)0x415c30;
  System__LStrClr_40929B27(&local_8,uStack_1c,puVar2);
  return;
}

