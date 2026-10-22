
void FUN_0043ad14(int param_1)

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
  puStack_18 = &LAB_0043aeee;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  FUN_0043ac88(param_1);
  *(undefined1 *)(param_1 + 0x2cc) = 0;
  (**(code **)(**(int **)(param_1 + 0x154) + 0x6c))(*(int **)(param_1 + 0x154),1);
  (**(code **)(**(int **)(param_1 + 0x138) + 0x6c))(*(int **)(param_1 + 0x138),1);
  (**(code **)(**(int **)(param_1 + 0x134) + 0x6c))(*(int **)(param_1 + 0x134),0);
  (**(code **)(**(int **)(param_1 + 0x148) + 0x6c))(*(int **)(param_1 + 0x148),0);
  (**(code **)(**(int **)(param_1 + 0x48) + 0x6c))(*(int **)(param_1 + 0x48),0);
  (**(code **)(**(int **)(param_1 + 0x104) + 0x6c))(*(int **)(param_1 + 0x104),0);
  (**(code **)(**(int **)(param_1 + 0x1fc) + 0x6c))(*(int **)(param_1 + 0x1fc),0);
  (**(code **)(**(int **)(param_1 + 300) + 0x6c))(*(int **)(param_1 + 300),1);
  System_LoadResString_02614E1B(AoWE_OptionsRStr_04ECFCC5_exref,&local_c);
  AoWE_TranslateRStr_435799AA(local_c,&local_8);
  AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 300),local_8);
  (**(code **)(**(int **)(param_1 + 0x150) + 0x6c))(*(int **)(param_1 + 0x150),0);
  (**(code **)(**(int **)(param_1 + 0x2a8) + 0x6c))
            (*(int **)(param_1 + 0x2a8),
             CONCAT31((int3)((uint)extraout_EDX >> 8),
                      *(undefined1 *)(*(int *)(param_1 + 0x150) + 0x75)) ^ 1);
  (**(code **)(**(int **)(param_1 + 0x2a4) + 0x6c))
            (*(int **)(param_1 + 0x2a4),
             CONCAT31((int3)((uint)extraout_EDX_00 >> 8),
                      *(undefined1 *)(*(int *)(param_1 + 0x150) + 0x75)) ^ 1);
  (**(code **)(**(int **)(param_1 + 0x120) + 0x6c))
            (*(int **)(param_1 + 0x120),
             *(char *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x6d) != '\0');
  (**(code **)(**(int **)(param_1 + 0x124) + 0x6c))
            (*(int **)(param_1 + 0x124),
             *(char *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x6d) != '\0');
  (**(code **)(**(int **)(param_1 + 0x11c) + 0x6c))
            (*(int **)(param_1 + 0x11c),
             *(char *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x6d) != '\0');
  *(undefined4 *)(param_1 + 0x2d4) = 0;
  *(undefined4 *)(param_1 + 0x2c4) = 0;
  AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x130),0xffffffff);
  FUN_0043ac6c(param_1);
  System_LoadResString_02614E1B(AoWE_QueueRStr_04ECFCC5_exref,&local_c);
  AoWE_TranslateRStr_435799AA(local_c,&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x198),local_8);
  System_LoadResString_02614E1B(AoWE_RepeatRStr_04ECFCC5_exref,&local_c);
  AoWE_TranslateRStr_435799AA(local_c,&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x1a0),local_8);
  if (*(int *)(param_1 + 0x2d0) != 0) {
    System_TObject_Free_23EDC2EF();
    *(undefined4 *)(param_1 + 0x2d0) = 0;
  }
  puVar1 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_0043aef5;
  puStack_18 = (undefined1 *)0x43aeed;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar1);
  return;
}

