
void FUN_0042d2cc(int param_1)

{
  int iVar1;
  undefined1 *puVar2;
  char cVar3;
  int iVar4;
  int *piVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  int iVar8;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_28;
  undefined1 *puStack_24;
  undefined1 *puStack_20;
  undefined4 local_10;
  int local_c;
  int local_8;
  
  puStack_20 = &stack0xfffffffc;
  local_10 = 0;
  puStack_24 = &LAB_0042d461;
  uStack_28 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_28;
  iVar1 = *(int *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x188);
  local_8 = param_1;
  (**(code **)(**(int **)(*(int *)(param_1 + 0xa0) + 0x114) + 0x40))();
  (**(code **)(**(int **)(*(int *)(local_8 + 0xb0) + 0x114) + 0x40))();
  (**(code **)(**(int **)(*(int *)(local_8 + 0xa8) + 0x114) + 0x40))();
  iVar4 = (**(code **)(**(int **)(iVar1 + 0x14) + 0x54))();
  if (-1 < iVar4 + -1) {
    iVar8 = 0;
    local_c = iVar4;
    do {
      piVar5 = (int *)AoWE_TGlobalEnchantmentList_GetEnchantments_23EDC2EF
                                (*(undefined4 *)(iVar1 + 0x14),iVar8);
      cVar3 = (**(code **)(*piVar5 + 0x84))
                        (piVar5,CONCAT31((int3)((uint)*(int *)AoWE_AoWHSMap_6B957A58_exref >> 8),
                                         *(undefined1 *)
                                          (*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5)));
      if (cVar3 != '\0') {
        piVar5 = (int *)AoWE_TGlobalEnchantmentList_GetEnchantments_23EDC2EF
                                  (*(undefined4 *)(iVar1 + 0x14),iVar8);
        (**(code **)(*piVar5 + 0x4c))(piVar5,&local_10);
        uVar7 = local_10;
        uVar6 = AoWE_TGlobalEnchantmentList_GetEnchantments_23EDC2EF
                          (*(undefined4 *)(iVar1 + 0x14),iVar8);
        piVar5 = *(int **)(*(int *)(local_8 + 0xa0) + 0x114);
        (**(code **)(*piVar5 + 0x38))(piVar5,uVar7,uVar6);
        iVar4 = AoWE_TGlobalEnchantmentList_GetEnchantments_23EDC2EF
                          (*(undefined4 *)(iVar1 + 0x14),iVar8);
        iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                          (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                           (int)*(char *)(iVar4 + 0x19));
        piVar5 = *(int **)(*(int *)(local_8 + 0xb0) + 0x114);
        (**(code **)(*piVar5 + 0x34))(piVar5,*(undefined4 *)(*(int *)(iVar4 + 0xd4) + 0x60));
        piVar5 = (int *)AoWE_TGlobalEnchantmentList_GetEnchantments_23EDC2EF
                                  (*(undefined4 *)(iVar1 + 0x14),iVar8);
        uVar7 = (**(code **)(*piVar5 + 0x50))();
        SysUtils_IntToStr_0F6FDFF6(uVar7,&local_10);
        piVar5 = *(int **)(*(int *)(local_8 + 0xa8) + 0x114);
        (**(code **)(*piVar5 + 0x34))(piVar5,local_10);
      }
      iVar8 = iVar8 + 1;
      local_c = local_c + -1;
    } while (local_c != 0);
  }
  FUN_0042cba4(local_8);
  FUN_0042d638(local_8,*(undefined4 *)(*(int *)(local_8 + 0xa0) + 0x120));
  puVar2 = puStack_20;
  *(undefined1 *)(*(int *)(local_8 + 0x44) + 0xa4) = 0;
  *in_FS_OFFSET = uStack_28;
  puStack_20 = &LAB_0042d468;
  puStack_24 = (undefined1 *)0x42d460;
  System__LStrClr_40929B27(&local_10,uStack_28,puVar2);
  return;
}

