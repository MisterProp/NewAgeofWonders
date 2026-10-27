
void FUN_0043efa0(int param_1)

{
  undefined1 *puVar1;
  undefined4 extraout_EDX;
  undefined4 extraout_EDX_00;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_14 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  puStack_18 = &LAB_0043f150;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  FUN_0043ef14(param_1);
  *(undefined1 *)(param_1 + 500) = 0;
  (**(code **)(**(int **)(param_1 + 0x124) + 0x6c))(*(int **)(param_1 + 0x124),1);
  (**(code **)(**(int **)(param_1 + 0x108) + 0x6c))(*(int **)(param_1 + 0x108),1);
  (**(code **)(**(int **)(param_1 + 0x104) + 0x6c))(*(int **)(param_1 + 0x104),0);
  (**(code **)(**(int **)(param_1 + 0x118) + 0x6c))(*(int **)(param_1 + 0x118),0);
  (**(code **)(**(int **)(param_1 + 0x48) + 0x6c))(*(int **)(param_1 + 0x48),0);
  (**(code **)(**(int **)(param_1 + 0xd4) + 0x6c))(*(int **)(param_1 + 0xd4),0);
  (**(code **)(**(int **)(param_1 + 0xfc) + 0x6c))(*(int **)(param_1 + 0xfc),1);
  System_LoadResString_02614E1B(AoWE_OptionsRStr_04ECFCC5_exref,&local_c);
  AoWE_TranslateRStr_435799AA(local_c,&local_8);
  AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xfc),local_8);
  (**(code **)(**(int **)(param_1 + 0x120) + 0x6c))(*(int **)(param_1 + 0x120),0);
  (**(code **)(**(int **)(param_1 + 0x1d8) + 0x6c))
            (*(int **)(param_1 + 0x1d8),
             CONCAT31((int3)((uint)extraout_EDX >> 8),
                      *(undefined1 *)(*(int *)(param_1 + 0x120) + 0x75)) ^ 1);
  (**(code **)(**(int **)(param_1 + 0x1d4) + 0x6c))
            (*(int **)(param_1 + 0x1d4),
             CONCAT31((int3)((uint)extraout_EDX_00 >> 8),
                      *(undefined1 *)(*(int *)(param_1 + 0x120) + 0x75)) ^ 1);
  (**(code **)(**(int **)(param_1 + 0xf0) + 0x6c))
            (*(int **)(param_1 + 0xf0),
             *(char *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x6d) != '\0');
  (**(code **)(**(int **)(param_1 + 0xf4) + 0x6c))
            (*(int **)(param_1 + 0xf4),
             *(char *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x6d) != '\0');
  (**(code **)(**(int **)(param_1 + 0xec) + 0x6c))
            (*(int **)(param_1 + 0xec),
             *(char *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x6d) != '\0');
  *(undefined4 *)(param_1 + 0x1f0) = 0xffffffff;
  AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x100),0xffffffff);
  System_LoadResString_02614E1B(AoWE_QueueRStr_04ECFCC5_exref,&local_c);
  AoWE_TranslateRStr_435799AA(local_c,&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x154),local_8);
  System_LoadResString_02614E1B(AoWE_RepeatRStr_04ECFCC5_exref,&local_c);
  AoWE_TranslateRStr_435799AA(local_c,&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x15c),local_8);
  FUN_0043f1d8(param_1);
  puVar1 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_0043f157;
  puStack_18 = (undefined1 *)0x43f14f;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar1);
  return;
}

