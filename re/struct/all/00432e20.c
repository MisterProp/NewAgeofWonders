
void FUN_00432e20(int param_1)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  undefined4 uVar5;
  undefined4 *in_FS_OFFSET;
  undefined4 uVar6;
  undefined4 uStack_28;
  undefined1 *puStack_24;
  undefined1 *puStack_20;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_20 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  local_10 = 0;
  local_14 = 0;
  puStack_24 = &LAB_00432f67;
  uStack_28 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_28;
  cVar1 = GlobalSpells_TDisjunctionSpellCaster_GetEnoughMana_23EDC2EF
                    (*(undefined4 *)(param_1 + 0xa4));
  if (cVar1 == '\0') {
    (**(code **)(**(int **)(param_1 + 0x68) + 0x70))(*(int **)(param_1 + 0x68),0xff);
  }
  else {
    (**(code **)(**(int **)(param_1 + 0x68) + 0x70))(*(int **)(param_1 + 0x68),0xc0c0c0);
  }
  iVar2 = AoWE_TSpellControl_GetSpell_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x84),0x33);
  SysUtils_IntToStr_0F6FDFF6(*(undefined4 *)(iVar2 + 0x14),&local_c);
  uVar3 = GlobalSpells_TDisjunctionSpellCaster_GetMana_23EDC2EF(*(undefined4 *)(param_1 + 0xa4));
  SysUtils_IntToStr_0F6FDFF6(uVar3,&local_10);
  uVar6 = local_10;
  iVar2 = AoWE_TSpellControl_GetSpell_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x84),0x33);
  iVar2 = *(int *)(iVar2 + 0x14);
  iVar4 = GlobalSpells_TDisjunctionSpellCaster_GetMana_23EDC2EF(*(undefined4 *)(param_1 + 0xa4));
  SysUtils_IntToStr_0F6FDFF6(iVar2 + iVar4,&local_14);
  uVar3 = local_14;
  System__LStrCatN_51F89FF7(&local_8,5);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x68),local_8);
  SysUtils_IntToStr_0F6FDFF6(*(undefined4 *)(*(int *)(param_1 + 0xa4) + 0x18),&local_8);
  System__LStrCat_51F89FF7(&local_8,&LAB_00432f94);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x5c),local_8);
  uVar5 = GlobalSpells_TDisjunctionSpellCaster_GetDispelChanceMax_23EDC2EF
                    (*(undefined4 *)(param_1 + 0xa4));
  AoWVScroll_TAOWScrollBar_SetFMax_23EDC2EF(*(undefined4 *)(param_1 + 0x90),uVar5);
  AoWVScroll_TAOWScrollBar_SetFPos_23EDC2EF
            (*(undefined4 *)(param_1 + 0x90),*(undefined4 *)(*(int *)(param_1 + 0xa4) + 0x18));
  *in_FS_OFFSET = uVar3;
  System__LStrArrayClr_51F89FF7(&local_14,4,uVar6,&LAB_00432f6e);
  return;
}

