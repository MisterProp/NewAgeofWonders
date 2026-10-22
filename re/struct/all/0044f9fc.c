
void FUN_0044f9fc(undefined4 param_1,undefined4 param_2)

{
  undefined *puVar1;
  undefined1 *puVar2;
  int iVar3;
  uint uVar4;
  char cVar5;
  int iVar6;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_24;
  undefined1 *puStack_20;
  undefined1 *puStack_1c;
  undefined4 local_c;
  char local_5;
  
  puVar1 = PTR_DAT_0045a0cc;
  puStack_1c = &stack0xfffffffc;
  local_c = 0;
  puStack_20 = &LAB_0044fc73;
  uStack_24 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_24;
  FUN_0042d480(*(undefined4 *)PTR_DAT_0045a394,param_2);
  FUN_0042e8b0(*(undefined4 *)PTR_DAT_0045a3e8,param_2);
  local_5 = '\0';
  cVar5 = (char)param_2;
  if (cVar5 == *(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5)) {
    local_5 = '\x01';
  }
  else if ((*(int *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x11c) != 0) &&
          (cVar5 == *(char *)(*(int *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x11c) + 0xa5))) {
    local_5 = '\x01';
  }
  if (local_5 != '\0') {
    if (cVar5 == -1) {
      local_5 = '\0';
    }
    else {
      iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),(int)cVar5);
      if (*(char *)(iVar3 + 0xa7) != '\0') {
        local_5 = '\0';
      }
    }
    if (local_5 == '\0') {
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(*(int *)puVar1 + 0xe8),0);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(*(int *)puVar1 + 0xe4),0);
    }
    else {
      iVar6 = (int)cVar5;
      iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),iVar6);
      SysUtils_IntToStr_0F6FDFF6(*(undefined4 *)(*(int *)(iVar3 + 0x54) + 0x24),&local_c);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(*(int *)puVar1 + 0xe8),local_c);
      iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),iVar6);
      iVar3 = AoWE_TPlayerMagicControl_GetManaIncome_23EDC2EF(*(undefined4 *)(iVar3 + 0x54));
      iVar6 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),iVar6);
      iVar6 = AoWE_TPlayerMagicControl_GetManaUpkeep_23EDC2EF(*(undefined4 *)(iVar6 + 0x54));
      uVar4 = iVar3 - iVar6;
      if ((int)uVar4 < 0) {
        AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(*(int *)puVar1 + 0xf4),&LAB_0044fc98);
        (**(code **)(**(int **)(*(int *)puVar1 + 0xf4) + 0x70))
                  (*(int **)(*(int *)puVar1 + 0xf4),0x77);
        (**(code **)(**(int **)(*(int *)puVar1 + 0xe4) + 0x70))
                  (*(int **)(*(int *)puVar1 + 0xe4),0x77);
      }
      else {
        AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(*(int *)puVar1 + 0xf4),&DAT_0044fc8c);
        (**(code **)(**(int **)(*(int *)puVar1 + 0xf4) + 0x70))
                  (*(int **)(*(int *)puVar1 + 0xf4),0x77ff77);
        (**(code **)(**(int **)(*(int *)puVar1 + 0xe4) + 0x70))
                  (*(int **)(*(int *)puVar1 + 0xe4),0x77ff77);
      }
      SysUtils_IntToStr_0F6FDFF6((uVar4 ^ (int)uVar4 >> 0x1f) - ((int)uVar4 >> 0x1f),&local_c);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(*(int *)puVar1 + 0xe4),local_c);
    }
    AoWLabel_TAOWLabel_SetGText_23EDC2EF
              (*(undefined4 *)(*(int *)puVar1 + 0x114),
               *(undefined4 *)(*(int *)(*(int *)puVar1 + 0xe8) + 0x90));
    AoWLabel_TAOWLabel_SetGText_23EDC2EF
              (*(undefined4 *)(*(int *)puVar1 + 0x110),
               *(undefined4 *)(*(int *)(*(int *)puVar1 + 0xe4) + 0x90));
    AoWLabel_TAOWLabel_SetGText_23EDC2EF
              (*(undefined4 *)(*(int *)puVar1 + 0x124),
               *(undefined4 *)(*(int *)(*(int *)puVar1 + 0xf4) + 0x90));
    (**(code **)(**(int **)(*(int *)puVar1 + 0x124) + 0x70))
              (*(int **)(*(int *)puVar1 + 0x124),
               *(undefined4 *)(*(int *)(*(int *)puVar1 + 0xf4) + 0x24));
    (**(code **)(**(int **)(*(int *)puVar1 + 0x110) + 0x70))
              (*(int **)(*(int *)puVar1 + 0x110),
               *(undefined4 *)(*(int *)(*(int *)puVar1 + 0xe4) + 0x24));
  }
  puVar2 = puStack_1c;
  *in_FS_OFFSET = uStack_24;
  puStack_1c = &LAB_0044fc7a;
  puStack_20 = (undefined1 *)0x44fc72;
  System__LStrClr_40929B27(&local_c,uStack_24,puVar2);
  return;
}

