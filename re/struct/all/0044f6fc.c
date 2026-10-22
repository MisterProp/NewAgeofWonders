
void FUN_0044f6fc(undefined4 param_1,char param_2)

{
  bool bVar1;
  undefined1 *puVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_20;
  undefined1 *puStack_1c;
  undefined1 *puStack_18;
  undefined4 local_c;
  char local_5;
  
  puStack_18 = &stack0xfffffffc;
  local_c = 0;
  puStack_1c = &LAB_0044f956;
  uStack_20 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_20;
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
      puStack_18 = &stack0xfffffffc;
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
      iVar4 = (int)local_5;
      iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),iVar4);
      iVar3 = *(int *)(iVar3 + 0xd0);
      AoWE_TPlayerList_GetPlayers_23EDC2EF
                (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),iVar4);
      iVar4 = AoWE_TPlayer_GetUpkeep_23EDC2EF();
      uVar5 = iVar3 - iVar4;
      if ((int)uVar5 < 0) {
        AoWLabel_TAOWLabel_SetGText_23EDC2EF
                  (*(undefined4 *)(*(int *)PTR_DAT_0045a0cc + 0xe0),&LAB_0044f978);
        (**(code **)(**(int **)(*(int *)PTR_DAT_0045a0cc + 0xe0) + 0x70))
                  (*(int **)(*(int *)PTR_DAT_0045a0cc + 0xe0),0x77);
        (**(code **)(**(int **)(*(int *)PTR_DAT_0045a0cc + 0x54) + 0x70))
                  (*(int **)(*(int *)PTR_DAT_0045a0cc + 0x54),0x77);
      }
      else {
        AoWLabel_TAOWLabel_SetGText_23EDC2EF
                  (*(undefined4 *)(*(int *)PTR_DAT_0045a0cc + 0xe0),&DAT_0044f96c);
        (**(code **)(**(int **)(*(int *)PTR_DAT_0045a0cc + 0xe0) + 0x70))
                  (*(int **)(*(int *)PTR_DAT_0045a0cc + 0xe0),0x77ff77);
        (**(code **)(**(int **)(*(int *)PTR_DAT_0045a0cc + 0x54) + 0x70))
                  (*(int **)(*(int *)PTR_DAT_0045a0cc + 0x54),0x77ff77);
      }
      SysUtils_IntToStr_0F6FDFF6((uVar5 ^ (int)uVar5 >> 0x1f) - ((int)uVar5 >> 0x1f),&local_c);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(*(int *)PTR_DAT_0045a0cc + 0x54),local_c)
      ;
      if ((*(char *)(*(int *)(*(int *)PTR_DAT_0045a0b0 + 0x44) + 0x75) != '\0') &&
         (*(int *)(*(int *)(*(int *)PTR_DAT_0045a0b0 + 0x50) + 0x118) == 0)) {
        FUN_00449b30(*(undefined4 *)PTR_DAT_0045a0b0);
      }
    }
    else {
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(*(int *)PTR_DAT_0045a0cc + 0x54),0);
    }
    AoWLabel_TAOWLabel_SetGText_23EDC2EF
              (*(undefined4 *)(*(int *)PTR_DAT_0045a0cc + 0xf8),
               *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a0cc + 0x54) + 0x90));
    AoWLabel_TAOWLabel_SetGText_23EDC2EF
              (*(undefined4 *)(*(int *)PTR_DAT_0045a0cc + 0x10c),
               *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a0cc + 0xe0) + 0x90));
    (**(code **)(**(int **)(*(int *)PTR_DAT_0045a0cc + 0x10c) + 0x70))
              (*(int **)(*(int *)PTR_DAT_0045a0cc + 0x10c),
               *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a0cc + 0xe0) + 0x24));
    (**(code **)(**(int **)(*(int *)PTR_DAT_0045a0cc + 0xf8) + 0x70))
              (*(int **)(*(int *)PTR_DAT_0045a0cc + 0xf8),
               *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a0cc + 0x54) + 0x24));
    puVar2 = puStack_18;
  }
  puStack_18 = puVar2;
  puVar2 = puStack_18;
  *in_FS_OFFSET = uStack_20;
  puStack_18 = &LAB_0044f95d;
  puStack_1c = (undefined1 *)0x44f955;
  System__LStrClr_40929B27(&local_c,uStack_20,puVar2);
  return;
}

