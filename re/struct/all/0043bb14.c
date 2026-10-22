
void FUN_0043bb14(int param_1,char param_2)

{
  int *piVar1;
  undefined1 *puVar2;
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
  puStack_18 = &LAB_0043be69;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  *(char *)(param_1 + 0x2cc) = param_2;
  if (param_2 == '\0') {
    (**(code **)(**(int **)(param_1 + 0x154) + 0x6c))(*(int **)(param_1 + 0x154),1);
    (**(code **)(**(int **)(param_1 + 0x138) + 0x6c))(*(int **)(param_1 + 0x138),1);
    (**(code **)(**(int **)(param_1 + 0x134) + 0x6c))(*(int **)(param_1 + 0x134),0);
    (**(code **)(**(int **)(param_1 + 0x148) + 0x6c))(*(int **)(param_1 + 0x148),0);
    System_LoadResString_02614E1B(AoWE_ChangeRStr_04ECFCC5_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_8);
    AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 300),local_8);
    (**(code **)(**(int **)(param_1 + 300) + 0x6c))(*(int **)(param_1 + 300),1);
    (**(code **)(**(int **)(param_1 + 0x150) + 0x6c))(*(int **)(param_1 + 0x150),0);
    (**(code **)(**(int **)(param_1 + 0x2a8) + 0x6c))
              (*(int **)(param_1 + 0x2a8),
               CONCAT31((int3)((uint)extraout_EDX >> 8),
                        *(undefined1 *)(*(int *)(param_1 + 0x150) + 0x75)) ^ 1);
    (**(code **)(**(int **)(param_1 + 0x2a4) + 0x6c))
              (*(int **)(param_1 + 0x2a4),
               CONCAT31((int3)((uint)extraout_EDX_00 >> 8),
                        *(undefined1 *)(*(int *)(param_1 + 0x150) + 0x75)) ^ 1);
    System_LoadResString_02614E1B(AoWE_ExitRStr_04ECFCC5_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_8);
    AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x128),local_8);
    AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x130),0xffffffff);
    (**(code **)(**(int **)(param_1 + 400) + 0x6c))(*(int **)(param_1 + 400),1);
    (**(code **)(**(int **)(param_1 + 0x18c) + 0x6c))(*(int **)(param_1 + 0x18c),1);
    piVar1 = *(int **)(param_1 + 0x29c);
    (**(code **)(*piVar1 + 100))(piVar1,*(int *)(*(int *)(param_1 + 0x154) + 0x88) - piVar1[0x22]);
  }
  else if (param_2 == '\x01') {
    puStack_14 = &stack0xfffffffc;
    AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x130),0xffffffff);
    (**(code **)(**(int **)(param_1 + 0x154) + 0x6c))(*(int **)(param_1 + 0x154),0);
    (**(code **)(**(int **)(param_1 + 0x138) + 0x6c))(*(int **)(param_1 + 0x138),0);
    (**(code **)(**(int **)(param_1 + 0x134) + 0x6c))(*(int **)(param_1 + 0x134),1);
    (**(code **)(**(int **)(param_1 + 0x148) + 0x6c))(*(int **)(param_1 + 0x148),1);
    System_LoadResString_02614E1B(AoWE_InfoRStr_8276FCC3_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_8);
    AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 300),local_8);
    System_LoadResString_02614E1B(AoWE_DoneRStr_04ECFCC5_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_8);
    AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x128),local_8);
    (**(code **)(**(int **)(param_1 + 400) + 0x6c))(*(int **)(param_1 + 400),0);
    (**(code **)(**(int **)(param_1 + 0x18c) + 0x6c))(*(int **)(param_1 + 0x18c),0);
    piVar1 = *(int **)(param_1 + 0x29c);
    (**(code **)(*piVar1 + 100))(piVar1,*(int *)(*(int *)(param_1 + 0x134) + 0x88) - piVar1[0x22]);
    *(undefined1 *)(*(int *)(param_1 + 0x230) + 0xc0) = 1;
    *(undefined1 *)(*(int *)(param_1 + 0x238) + 0xc0) = 1;
    *(undefined1 *)(*(int *)(param_1 + 0x234) + 0xc0) = 1;
  }
  else {
    puStack_14 = &stack0xfffffffc;
    if (param_2 == '\x02') {
      puStack_14 = &stack0xfffffffc;
      FUN_0043c2ec(param_1);
      AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x130),0xffffffff);
      (**(code **)(**(int **)(param_1 + 0x154) + 0x6c))(*(int **)(param_1 + 0x154),0);
      (**(code **)(**(int **)(param_1 + 0x138) + 0x6c))(*(int **)(param_1 + 0x138),0);
      (**(code **)(**(int **)(param_1 + 0x134) + 0x6c))(*(int **)(param_1 + 0x134),1);
      (**(code **)(**(int **)(param_1 + 0x148) + 0x6c))(*(int **)(param_1 + 0x148),1);
      (**(code **)(**(int **)(param_1 + 400) + 0x6c))(*(int **)(param_1 + 400),0);
      (**(code **)(**(int **)(param_1 + 0x18c) + 0x6c))(*(int **)(param_1 + 0x18c),0);
      System_LoadResString_02614E1B(AoWE_InfoRStr_8276FCC3_exref,&local_c);
      AoWE_TranslateRStr_435799AA(local_c,&local_8);
      AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 300),local_8);
      System_LoadResString_02614E1B(AoWE_DoneRStr_04ECFCC5_exref,&local_c);
      AoWE_TranslateRStr_435799AA(local_c,&local_8);
      AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x128),local_8);
      *(undefined1 *)(*(int *)(param_1 + 0x230) + 0xc0) = 0;
      *(undefined1 *)(*(int *)(param_1 + 0x238) + 0xc0) = 0;
      *(undefined1 *)(*(int *)(param_1 + 0x234) + 0xc0) = 0;
      AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x230),0xffffffff);
      AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x238),0xffffffff);
      AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x234),0xffffffff);
    }
  }
  puVar2 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_0043be70;
  puStack_18 = (undefined1 *)0x43be68;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar2);
  return;
}

