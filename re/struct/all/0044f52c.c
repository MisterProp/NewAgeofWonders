
void FUN_0044f52c(undefined4 param_1,char param_2)

{
  bool bVar1;
  undefined1 *puVar2;
  int iVar3;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_c;
  char local_5;
  
  puStack_14 = &stack0xfffffffc;
  local_c = 0;
  puStack_18 = &LAB_0044f6de;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  bVar1 = false;
  if (*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5) == param_2) {
    bVar1 = true;
  }
  else if ((*(int *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x11c) != 0) &&
          (*(char *)(*(int *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x11c) + 0xa5) == param_2)) {
    bVar1 = true;
  }
  puVar2 = &stack0xfffffffc;
  local_5 = param_2;
  if (bVar1) {
    if (param_2 == -1) {
      bVar1 = false;
      puStack_14 = &stack0xfffffffc;
    }
    else {
      iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),(int)param_2)
      ;
      bVar1 = true;
      if (*(char *)(iVar3 + 0xa7) != '\0') {
        bVar1 = false;
      }
    }
    if (bVar1) {
      iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),(int)local_5)
      ;
      SysUtils_IntToStr_0F6FDFF6(*(undefined4 *)(iVar3 + 0xc4),&local_c);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(*(int *)PTR_DAT_0045a0cc + 0x58),local_c)
      ;
      if ((*(char *)(*(int *)(*(int *)PTR_DAT_0045a0b0 + 0x44) + 0x75) != '\0') &&
         (*(int *)(*(int *)(*(int *)PTR_DAT_0045a0b0 + 0x50) + 0x118) == 0)) {
        FUN_00449b30(*(undefined4 *)PTR_DAT_0045a0b0);
      }
    }
    else {
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(*(int *)PTR_DAT_0045a0cc + 0x58),0);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(*(int *)PTR_DAT_0045a0cc + 0xe0),0);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(*(int *)PTR_DAT_0045a0cc + 0xf4),0);
    }
    AoWLabel_TAOWLabel_SetGText_23EDC2EF
              (*(undefined4 *)(*(int *)PTR_DAT_0045a0cc + 0xfc),
               *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a0cc + 0x58) + 0x90));
    AoWLabel_TAOWLabel_SetGText_23EDC2EF
              (*(undefined4 *)(*(int *)PTR_DAT_0045a0cc + 0x10c),
               *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a0cc + 0xe0) + 0x90));
    AoWLabel_TAOWLabel_SetGText_23EDC2EF
              (*(undefined4 *)(*(int *)PTR_DAT_0045a0cc + 0x124),
               *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a0cc + 0xf4) + 0x90));
    puVar2 = puStack_14;
  }
  puStack_14 = puVar2;
  puVar2 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_0044f6e5;
  puStack_18 = (undefined1 *)0x44f6dd;
  System__LStrClr_40929B27(&local_c,uStack_1c,puVar2);
  return;
}

