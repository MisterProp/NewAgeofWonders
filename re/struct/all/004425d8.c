
void FUN_004425d8(int param_1)

{
  undefined1 *puVar1;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_14 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  puStack_18 = &LAB_0044274f;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  (**(code **)(**(int **)(param_1 + 0x60) + 0x6c))(*(int **)(param_1 + 0x60),1);
  *(undefined1 *)(*(int *)(param_1 + 0x60) + 0x2e) = 0;
  *(undefined1 *)(*(int *)(param_1 + 0x44) + 0x2d) = 0;
  (**(code **)(**(int **)(param_1 + 0x9c) + 0x6c))(*(int **)(param_1 + 0x9c),0);
  (**(code **)(**(int **)(param_1 + 0xa4) + 0x6c))(*(int **)(param_1 + 0xa4),0);
  (**(code **)(**(int **)(param_1 + 0x68) + 0x6c))(*(int **)(param_1 + 0x68),0);
  (**(code **)(**(int **)(param_1 + 0x74) + 0x6c))(*(int **)(param_1 + 0x74),0);
  (**(code **)(**(int **)(param_1 + 0xd8) + 0x6c))(*(int **)(param_1 + 0xd8),0);
  (**(code **)(**(int **)(param_1 + 0xac) + 0x6c))(*(int **)(param_1 + 0xac),0);
  (**(code **)(**(int **)(param_1 + 0x58) + 0x6c))(*(int **)(param_1 + 0x58),1);
  (**(code **)(**(int **)(param_1 + 0x114) + 0x6c))(*(int **)(param_1 + 0x114),1);
  (**(code **)(**(int **)(param_1 + 0x18c) + 0x6c))(*(int **)(param_1 + 0x18c),0);
  (**(code **)(**(int **)(param_1 + 0xb4) + 0x6c))(*(int **)(param_1 + 0xb4),0);
  (**(code **)(**(int **)(param_1 + 0x158) + 0x6c))(*(int **)(param_1 + 0x158),0);
  (**(code **)(**(int **)(param_1 + 0x170) + 0x6c))(*(int **)(param_1 + 0x170),0);
  (**(code **)(**(int **)(param_1 + 0xfc) + 0x6c))(*(int **)(param_1 + 0xfc),0);
  (**(code **)(**(int **)(param_1 + 0xa8) + 0x6c))(*(int **)(param_1 + 0xa8),0);
  (**(code **)(**(int **)(param_1 + 0xf4) + 0x6c))(*(int **)(param_1 + 0xf4),0);
  (**(code **)(**(int **)(param_1 + 0xe8) + 0x6c))(*(int **)(param_1 + 0xe8),0);
  (**(code **)(**(int **)(param_1 + 0x10c) + 0x6c))(*(int **)(param_1 + 0x10c),0);
  (**(code **)(**(int **)(param_1 + 0x58) + 0x6c))(*(int **)(param_1 + 0x58),0);
  (**(code **)(**(int **)(*(int *)(param_1 + 0xb0) + 0x118) + 0x40))();
  (**(code **)(**(int **)(param_1 + 0x1a4) + 0x6c))(*(int **)(param_1 + 0x1a4),0);
  (**(code **)(**(int **)(param_1 + 0x1a8) + 0x6c))(*(int **)(param_1 + 0x1a8),0);
  System_LoadResString_02614E1B(AoWE_StructureRStr_8276FCC3_exref,&local_c);
  AoWE_TranslateRStr_435799AA(local_c,&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x50),local_8);
  puVar1 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_00442756;
  puStack_18 = (undefined1 *)0x44274e;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar1);
  return;
}

