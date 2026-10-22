
void FUN_0041e51c(int param_1)

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
  puStack_18 = &LAB_0041e616;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  if (*(char *)(*(int *)(param_1 + 0x8c) + 0x75) == '\0') {
    (**(code **)(**(int **)(param_1 + 0x104) + 0x6c))(*(int **)(param_1 + 0x104),0);
    (**(code **)(**(int **)(param_1 + 0x5c) + 0x6c))(*(int **)(param_1 + 0x5c),0);
    (**(code **)(**(int **)(param_1 + 0x60) + 0x6c))(*(int **)(param_1 + 0x60),0);
    (**(code **)(**(int **)(param_1 + 0x50) + 0x6c))(*(int **)(param_1 + 0x50),0);
    (**(code **)(**(int **)(param_1 + 0x54) + 0x6c))(*(int **)(param_1 + 0x54),0);
    (**(code **)(**(int **)(param_1 + 0x58) + 0x6c))(*(int **)(param_1 + 0x58),0);
    (**(code **)(**(int **)(param_1 + 0x8c) + 0x6c))(*(int **)(param_1 + 0x8c),1);
    System_LoadResString_02614E1B(PTR_PTR_0045a128,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_8);
    AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x100),local_8);
  }
  else {
    puStack_14 = &stack0xfffffffc;
    FUN_0041ca5c(param_1);
    (**(code **)(**(int **)(param_1 + 0x104) + 0x6c))(*(int **)(param_1 + 0x104),1);
    (**(code **)(**(int **)(param_1 + 0x8c) + 0x6c))(*(int **)(param_1 + 0x8c),0);
    System_LoadResString_02614E1B(PTR_PTR_0045a5b8,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_8);
    AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x100),local_8);
  }
  puVar1 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_0041e61d;
  puStack_18 = (undefined1 *)0x41e615;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar1);
  return;
}

