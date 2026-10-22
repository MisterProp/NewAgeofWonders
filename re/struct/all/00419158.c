
void FUN_00419158(int param_1)

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
  puStack_18 = &LAB_00419257;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  (**(code **)(**(int **)(*(int *)PTR_DAT_0045a28c + 0x44) + 0x6c))
            (*(int **)(*(int *)PTR_DAT_0045a28c + 0x44),1);
  (**(code **)(**(int **)(DAT_0045b0e0 + 0x44) + 0x6c))(*(int **)(DAT_0045b0e0 + 0x44),1);
  (**(code **)(**(int **)(param_1 + 0x48) + 0x6c))(*(int **)(param_1 + 0x48),1);
  System_LoadResString_02614E1B(PTR_PTR_0045a3b4,&local_c);
  AoWE_TranslateRStr_435799AA(local_c,&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x48),local_8);
  (**(code **)(**(int **)(param_1 + 0x6c) + 0x6c))(*(int **)(param_1 + 0x6c),0);
  (**(code **)(**(int **)(param_1 + 0x50) + 0x6c))(*(int **)(param_1 + 0x50),0);
  (**(code **)(**(int **)(param_1 + 0x58) + 0x6c))(*(int **)(param_1 + 0x58),0);
  (**(code **)(**(int **)(param_1 + 0x54) + 0x6c))(*(int **)(param_1 + 0x54),0);
  (**(code **)(**(int **)(param_1 + 0x5c) + 0x6c))(*(int **)(param_1 + 0x5c),0);
  (**(code **)(**(int **)(param_1 + 0x74) + 0x6c))(*(int **)(param_1 + 0x74),0);
  (**(code **)(**(int **)(param_1 + 0x70) + 0x6c))(*(int **)(param_1 + 0x70),0);
  (**(code **)(**(int **)(param_1 + 0x68) + 0x6c))(*(int **)(param_1 + 0x68),0);
  (**(code **)(**(int **)(param_1 + 100) + 0x6c))(*(int **)(param_1 + 100),0);
  (**(code **)(**(int **)(param_1 + 0x78) + 0x6c))(*(int **)(param_1 + 0x78),0);
  *(undefined4 *)(param_1 + 0x80) = 0xffffffff;
  AoWBaseWin_TAOWBaseWindow_SetILTIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x44),0);
  puVar1 = puStack_14;
  *(undefined1 *)(param_1 + 0x8c) = 0;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_0041925e;
  puStack_18 = (undefined1 *)0x419256;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar1);
  return;
}

