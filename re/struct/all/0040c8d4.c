
void FUN_0040c8d4(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  undefined4 *in_FS_OFFSET;
  undefined4 uVar5;
  undefined4 uStack_24;
  undefined1 *puStack_20;
  undefined1 *puStack_1c;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_1c = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  local_10 = 0;
  puStack_20 = &LAB_0040cb1e;
  uStack_24 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_24;
  iVar1 = *(int *)(*(int *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x70) + 0x20);
  SysUtils_IntToStr_0F6FDFF6(*(undefined4 *)(iVar1 + 0x4c),&local_c);
  uVar5 = local_c;
  SysUtils_IntToStr_0F6FDFF6(*(undefined4 *)(iVar1 + 0x40),&local_10);
  uVar2 = local_10;
  System__LStrCatN_51F89FF7(&local_8,3);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x98),local_8);
  SysUtils_IntToStr_0F6FDFF6(*(undefined4 *)(iVar1 + 0x38),&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x194),local_8);
  SysUtils_IntToStr_0F6FDFF6(*(undefined4 *)(iVar1 + 0x34),&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x18c),local_8);
  SysUtils_IntToStr_0F6FDFF6(*(int *)(iVar1 + 0x48) - *(int *)(iVar1 + 0x38),&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xac),local_8);
  SysUtils_IntToStr_0F6FDFF6(*(int *)(iVar1 + 0x44) - *(int *)(iVar1 + 0x34),&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xa4),local_8);
  AoWVScroll_TAOWScrollBar_SetFPos_23EDC2EF
            (*(undefined4 *)(param_1 + 0x13c),*(undefined4 *)(iVar1 + 0x38));
  AoWVScroll_TAOWScrollBar_SetFPos_23EDC2EF
            (*(undefined4 *)(param_1 + 0x138),*(undefined4 *)(iVar1 + 0x34));
  (**(code **)(**(int **)(param_1 + 0x140) + 0x6c))
            (*(int **)(param_1 + 0x140),*(int *)(param_1 + 0x1a8) != 0);
  iVar3 = (**(code **)(**(int **)(iVar1 + 0xc) + 0x54))();
  iVar4 = *(int *)(param_1 + 0x1a8) + 4;
  (**(code **)(**(int **)(param_1 + 0x150) + 0x6c))
            (*(int **)(param_1 + 0x150),CONCAT31((int3)((uint)iVar4 >> 8),iVar4 < iVar3));
  (**(code **)(**(int **)(param_1 + 0x148) + 0x6c))
            (*(int **)(param_1 + 0x148),*(int *)(param_1 + 0x1b8) != 0);
  iVar3 = (**(code **)(**(int **)(iVar1 + 0x10) + 0x54))();
  iVar4 = *(int *)(param_1 + 0x1b8) + 4;
  (**(code **)(**(int **)(param_1 + 0x154) + 0x6c))
            (*(int **)(param_1 + 0x154),CONCAT31((int3)((uint)iVar4 >> 8),iVar4 < iVar3));
  (**(code **)(**(int **)(param_1 + 0x14c) + 0x6c))
            (*(int **)(param_1 + 0x14c),*(int *)(param_1 + 0x1b0) != 0);
  iVar3 = (**(code **)(**(int **)(iVar1 + 0x14) + 0x54))();
  iVar4 = *(int *)(param_1 + 0x1b0) + 4;
  (**(code **)(**(int **)(param_1 + 0x158) + 0x6c))
            (*(int **)(param_1 + 0x158),CONCAT31((int3)((uint)iVar4 >> 8),iVar4 < iVar3));
  (**(code **)(**(int **)(param_1 + 0x15c) + 0x6c))
            (*(int **)(param_1 + 0x15c),*(int *)(param_1 + 0x1ac) != 0);
  (**(code **)(**(int **)(param_1 + 0x168) + 0x6c))
            (*(int **)(param_1 + 0x168),
             CONCAT31((int3)((uint)*(int *)(iVar1 + 0x28) >> 8),
                      *(int *)(param_1 + 0x1ac) + 4 < *(int *)(*(int *)(iVar1 + 0x28) + 8)));
  (**(code **)(**(int **)(param_1 + 0x160) + 0x6c))
            (*(int **)(param_1 + 0x160),*(int *)(param_1 + 0x1bc) != 0);
  (**(code **)(**(int **)(param_1 + 0x16c) + 0x6c))
            (*(int **)(param_1 + 0x16c),
             CONCAT31((int3)((uint)*(int *)(iVar1 + 0x2c) >> 8),
                      *(int *)(param_1 + 0x1bc) + 4 < *(int *)(*(int *)(iVar1 + 0x2c) + 8)));
  (**(code **)(**(int **)(param_1 + 0x164) + 0x6c))
            (*(int **)(param_1 + 0x164),*(int *)(param_1 + 0x1b4) != 0);
  (**(code **)(**(int **)(param_1 + 0x170) + 0x6c))
            (*(int **)(param_1 + 0x170),
             CONCAT31((int3)((uint)*(int *)(iVar1 + 0x30) >> 8),
                      *(int *)(param_1 + 0x1b4) + 4 < *(int *)(*(int *)(iVar1 + 0x30) + 8)));
  *(undefined1 *)(*(int *)(param_1 + 0x80) + 0xa4) = 0;
  *in_FS_OFFSET = uVar2;
  System__LStrArrayClr_51F89FF7(&local_10,3,uVar5,&LAB_0040cb25);
  return;
}

