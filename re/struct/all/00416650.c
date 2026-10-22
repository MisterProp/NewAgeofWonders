
void FUN_00416650(int param_1,int param_2)

{
  undefined1 *puVar1;
  undefined4 extraout_EDX;
  undefined4 extraout_EDX_00;
  undefined4 uVar2;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_20;
  undefined1 *puStack_1c;
  undefined1 *puStack_18;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_18 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  puStack_1c = &LAB_0041687f;
  uStack_20 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_20;
  *(int *)(param_1 + 0x2dc) = param_2;
  (**(code **)(**(int **)(param_1 + 0x188) + 0x6c))(*(int **)(param_1 + 0x188),param_2 == 1);
  (**(code **)(**(int **)(param_1 + 0x48) + 0x6c))(*(int **)(param_1 + 0x48),param_2 == 2);
  (**(code **)(**(int **)(param_1 + 0x1c8) + 0x6c))(*(int **)(param_1 + 0x1c8),param_2 == 3);
  (**(code **)(**(int **)(param_1 + 0x1b8) + 0x6c))(*(int **)(param_1 + 0x1b8),param_2 == 1);
  (**(code **)(**(int **)(param_1 + 0x2c4) + 0x6c))(*(int **)(param_1 + 0x2c4),param_2 == 2);
  (**(code **)(**(int **)(param_1 + 0x2c8) + 0x6c))(*(int **)(param_1 + 0x2c8),param_2 == 3);
  (**(code **)(**(int **)(param_1 + 0x144) + 0x6c))(*(int **)(param_1 + 0x144),2 < param_2);
  (**(code **)(**(int **)(param_1 + 0x14c) + 0x6c))(*(int **)(param_1 + 0x14c),2 < param_2);
  (**(code **)(**(int **)(param_1 + 0x158) + 0x6c))(*(int **)(param_1 + 0x158),1 < param_2);
  (**(code **)(**(int **)(param_1 + 0x230) + 0x6c))
            (*(int **)(param_1 + 0x230),
             CONCAT31((int3)((uint)extraout_EDX >> 8),
                      (*(byte *)(*(int *)(param_1 + 0x2d8) + 0x10) & 8) != 0) ^ 1);
  (**(code **)(**(int **)(param_1 + 0x234) + 0x6c))
            (*(int **)(param_1 + 0x234),
             CONCAT31((int3)((uint)extraout_EDX_00 >> 8),
                      (*(byte *)(*(int *)(param_1 + 0x2d8) + 0x10) & 8) != 0) ^ 1);
  if (((*(byte *)(*(int *)(param_1 + 0x2d8) + 0x10) & 8) == 0) || (1 < param_2)) {
    uVar2 = 1;
  }
  else {
    uVar2 = 0;
  }
  (**(code **)(**(int **)(param_1 + 0x15c) + 0x6c))(*(int **)(param_1 + 0x15c),uVar2);
  if (((*(byte *)(*(int *)(param_1 + 0x2d8) + 0x10) & 8) == 0) || (1 < param_2)) {
    uVar2 = 1;
  }
  else {
    uVar2 = 0;
  }
  (**(code **)(**(int **)(param_1 + 0x160) + 0x6c))(*(int **)(param_1 + 0x160),uVar2);
  if (((*(byte *)(*(int *)(param_1 + 0x2d8) + 0x10) & 8) == 0) || (1 < param_2)) {
    uVar2 = 1;
  }
  else {
    uVar2 = 0;
  }
  (**(code **)(**(int **)(param_1 + 0x2a4) + 0x6c))(*(int **)(param_1 + 0x2a4),uVar2);
  (**(code **)(**(int **)(param_1 + 0x184) + 0x6c))
            (*(int **)(param_1 + 0x184),*(undefined1 *)(*(int *)(param_1 + 0x2a4) + 0x75));
  if (((param_2 == 3) ||
      ((param_2 == 2 && ((*(byte *)(*(int *)(param_1 + 0x2d8) + 0x10) & 4) == 0)))) ||
     ((param_2 == 1 && ((*(byte *)(*(int *)(param_1 + 0x2d8) + 0x10) & 6) == 0)))) {
    System_LoadResString_02614E1B(AoWE_FinishRStr_04ECFCC5_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_8);
    AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x50),local_8);
  }
  else {
    System_LoadResString_02614E1B(AoWE_NextRStr_04ECFCC5_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_8);
    AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x50),local_8);
  }
  if (param_2 == 1) {
    FUN_00415f94(param_1);
  }
  else if (param_2 == 2) {
    FUN_00415f94(param_1);
  }
  else if (param_2 == 3) {
    FUN_00417658(param_1);
  }
  puVar1 = puStack_18;
  *in_FS_OFFSET = uStack_20;
  puStack_18 = &LAB_00416886;
  puStack_1c = (undefined1 *)0x41687e;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar1);
  return;
}

