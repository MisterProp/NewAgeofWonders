
void FUN_0043fd4c(int param_1,char param_2)

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
  puStack_18 = &LAB_0043ff9b;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  *(char *)(param_1 + 500) = param_2;
  if (param_2 == '\0') {
    (**(code **)(**(int **)(param_1 + 0x124) + 0x6c))(*(int **)(param_1 + 0x124),1);
    (**(code **)(**(int **)(param_1 + 0x108) + 0x6c))(*(int **)(param_1 + 0x108),1);
    (**(code **)(**(int **)(param_1 + 0x104) + 0x6c))(*(int **)(param_1 + 0x104),0);
    (**(code **)(**(int **)(param_1 + 0x118) + 0x6c))(*(int **)(param_1 + 0x118),0);
    System_LoadResString_02614E1B(AoWE_ChangeRStr_04ECFCC5_exref,&local_c);
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
    System_LoadResString_02614E1B(AoWE_ExitRStr_04ECFCC5_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_8);
    AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xf8),local_8);
    AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x100),0xffffffff);
  }
  else if (param_2 == '\x01') {
    puStack_14 = &stack0xfffffffc;
    FUN_0043f4a4(param_1);
    AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x100),0xffffffff);
    (**(code **)(**(int **)(param_1 + 0x124) + 0x6c))(*(int **)(param_1 + 0x124),0);
    (**(code **)(**(int **)(param_1 + 0x108) + 0x6c))(*(int **)(param_1 + 0x108),0);
    (**(code **)(**(int **)(param_1 + 0x104) + 0x6c))(*(int **)(param_1 + 0x104),1);
    (**(code **)(**(int **)(param_1 + 0x118) + 0x6c))(*(int **)(param_1 + 0x118),1);
    System_LoadResString_02614E1B(AoWE_InfoRStr_8276FCC3_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_8);
    AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xfc),local_8);
    System_LoadResString_02614E1B(AoWE_DoneRStr_04ECFCC5_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_8);
    AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xf8),local_8);
  }
  else {
    puStack_14 = &stack0xfffffffc;
    if (param_2 == '\x02') {
      puStack_14 = &stack0xfffffffc;
      FUN_0043f4a4(param_1);
      AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x100),0xffffffff);
      (**(code **)(**(int **)(param_1 + 0x124) + 0x6c))(*(int **)(param_1 + 0x124),0);
      (**(code **)(**(int **)(param_1 + 0x108) + 0x6c))(*(int **)(param_1 + 0x108),0);
      (**(code **)(**(int **)(param_1 + 0x104) + 0x6c))(*(int **)(param_1 + 0x104),1);
      (**(code **)(**(int **)(param_1 + 0x118) + 0x6c))(*(int **)(param_1 + 0x118),1);
      System_LoadResString_02614E1B(AoWE_InfoRStr_8276FCC3_exref,&local_c);
      AoWE_TranslateRStr_435799AA(local_c,&local_8);
      AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xfc),local_8);
      System_LoadResString_02614E1B(AoWE_DoneRStr_04ECFCC5_exref,&local_c);
      AoWE_TranslateRStr_435799AA(local_c,&local_8);
      AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xf8),local_8);
    }
  }
  puVar1 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_0043ffa2;
  puStack_18 = (undefined1 *)0x43ff9a;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar1);
  return;
}

