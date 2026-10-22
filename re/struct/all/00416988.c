
void FUN_00416988(int param_1,int *param_2)

{
  int *piVar1;
  undefined1 *puVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 extraout_EDX;
  int iVar5;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_24;
  undefined1 *puStack_20;
  undefined1 *puStack_1c;
  undefined4 local_c;
  int local_8;
  
  puStack_1c = &stack0xfffffffc;
  local_c = 0;
  puStack_20 = &LAB_00416bde;
  uStack_24 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_24;
  FUN_004168a4(param_1);
  if (*(int **)(param_1 + 0x2d8) != (int *)0x0) {
    (**(code **)(**(int **)(param_1 + 0x2d8) + 0x2c))();
  }
  *(int **)(param_1 + 0x2d8) = param_2;
  (**(code **)(*param_2 + 0x28))();
  *(undefined4 *)(param_1 + 0x2e8) = 0;
  *(undefined4 *)(param_1 + 0x2ec) = 0;
  (**(code **)(**(int **)(param_1 + 0x2cc) + 0xa4))
            (*(int **)(param_1 + 0x2cc),*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0xa4));
  AoWBaseWin_TAOWBaseWindow_SetILTIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x2cc),4);
  iVar3 = *(int *)(*(int *)(param_1 + 0x2d8) + 0x18);
  if ((*(int *)(iVar3 + 8) == 0) || ((*(byte *)(*(int *)(param_1 + 0x2d8) + 0x10) & 8) == 0)) {
    uVar4 = 0;
  }
  else {
    uVar4 = CONCAT31((int3)((uint)iVar3 >> 8),1);
  }
  (**(code **)(**(int **)(param_1 + 0x2d0) + 0x6c))(*(int **)(param_1 + 0x2d0),uVar4);
  (**(code **)(**(int **)(param_1 + 0x1b4) + 0x6c))
            (*(int **)(param_1 + 0x1b4),*(undefined1 *)(*(int *)(param_1 + 0x2d0) + 0x75));
  if (*(char *)(*(int *)(param_1 + 0x2d0) + 0x75) != '\0') {
    (**(code **)(**(int **)(*(int *)(param_1 + 0x2d0) + 0x11c) + 0x40))();
    iVar3 = *(int *)(*(int *)(*(int *)(param_1 + 0x2d8) + 0x18) + 8);
    if (-1 < iVar3 + -1) {
      iVar5 = 0;
      local_8 = iVar3;
      do {
        uVar4 = Engine_TIntegerList_Get_23EDC2EF
                          (*(undefined4 *)(*(int *)(param_1 + 0x2d8) + 0x18),iVar5);
        iVar3 = Engine_TIntegerList_Get_23EDC2EF
                          (*(undefined4 *)(*(int *)(param_1 + 0x2d8) + 0x18),iVar5);
        AoWE_TranslateRStr_435799AA
                  (*(undefined4 *)(AoWE_RaceNamesRStr_B11890CE_exref + iVar3 * 4),&local_c);
        piVar1 = *(int **)(*(int *)(param_1 + 0x2d0) + 0x11c);
        (**(code **)(*piVar1 + 0x38))(piVar1,local_c,uVar4);
        iVar5 = iVar5 + 1;
        local_8 = local_8 + -1;
      } while (local_8 != 0);
    }
    uVar4 = Engine_TIntegerList_IndexOf_23EDC2EF
                      (*(undefined4 *)(*(int *)(param_1 + 0x2d8) + 0x18),
                       (int)*(char *)(*(int *)(*(int *)(param_1 + 0x2d8) + 0x14) + 0x68));
    (**(code **)(**(int **)(param_1 + 0x2d0) + 0xb8))(*(int **)(param_1 + 0x2d0),uVar4);
  }
  AoWE_TranslateRStr_435799AA
            (*(undefined4 *)
              (AoWE_RaceNamesPluralRStr_B11890CE_exref +
              *(char *)(*(int *)(*(int *)(param_1 + 0x2d8) + 0x14) + 0x68) * 4),&local_c);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x234),local_c);
  AoWImage_TAOWImage_SetCurrent_23EDC2EF
            (*(undefined4 *)(param_1 + 0x15c),
             *(char *)(*(int *)(*(int *)(param_1 + 0x2d8) + 0x14) + 0x68) + 0xd);
  AoWImage_TAOWImage_SetCurrent_23EDC2EF
            (*(undefined4 *)(param_1 + 0x160),
             *(char *)(*(int *)(*(int *)(param_1 + 0x2d8) + 0x14) + 0x68) + 0xd);
  (**(code **)(**(int **)(param_1 + 0x160) + 0x6c))
            (*(int **)(param_1 + 0x160),
             CONCAT31((int3)((uint)extraout_EDX >> 8),
                      (*(byte *)(*(int *)(param_1 + 0x2d8) + 0x10) & 8) != 0) ^ 1);
  FUN_00415a74(param_1);
  FUN_00415a94(param_1);
  if ((*(byte *)(*(int *)(param_1 + 0x2d8) + 0x10) & 1) == 0) {
    if ((*(byte *)(*(int *)(param_1 + 0x2d8) + 0x10) & 2) == 0) {
      if ((*(byte *)(*(int *)(param_1 + 0x2d8) + 0x10) & 4) != 0) {
        FUN_00416650(param_1,3);
      }
    }
    else {
      FUN_00416650(param_1,2);
    }
  }
  else {
    FUN_00416650(param_1,1);
  }
  (**(code **)(**(int **)(param_1 + 0x2cc) + 0x6c))(*(int **)(param_1 + 0x2cc),1);
  puVar2 = puStack_1c;
  *in_FS_OFFSET = uStack_24;
  puStack_1c = &LAB_00416be5;
  puStack_20 = (undefined1 *)0x416bdd;
  System__LStrClr_40929B27(&local_c,uStack_24,puVar2);
  return;
}

