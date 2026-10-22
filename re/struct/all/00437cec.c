
void FUN_00437cec(int param_1,int *param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5)

{
  undefined1 *puVar1;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_20;
  undefined1 *puStack_1c;
  undefined1 *puStack_18;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_18 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  puStack_1c = &LAB_00437dcf;
  uStack_20 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_20;
  FUN_00437abc(param_1);
  (**(code **)(**(int **)(param_1 + 0x80) + 0x6c))(*(int **)(param_1 + 0x80),1);
  System_LoadResString_02614E1B(AoWE_PrisonerLiberationRStr_04ECFCC5_exref,&local_c);
  AoWE_TranslateRStr_435799AA(local_c,&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x70),local_8);
  *(int **)(param_1 + 0xac) = param_2;
  (**(code **)(*param_2 + 0x28))();
  *(undefined4 *)(param_1 + 0xa4) = param_4;
  *(undefined4 *)(param_1 + 0xa8) = param_5;
  System_LoadResString_02614E1B(AoWE_AcceptRStr_04ECFCC5_exref,&local_c);
  AoWE_TranslateRStr_435799AA(local_c,&local_8);
  AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x7c),local_8);
  (**(code **)(**(int **)(param_1 + 0x68) + 0x6c))(*(int **)(param_1 + 0x68),0);
  System_LoadResString_02614E1B(AoWE_RejectRStr_04ECFCC5_exref,&local_c);
  AoWE_TranslateRStr_435799AA(local_c,&local_8);
  AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x78),local_8);
  FUN_00437b3c(param_1);
  puVar1 = puStack_18;
  *in_FS_OFFSET = uStack_20;
  puStack_18 = &LAB_00437dd6;
  puStack_1c = (undefined1 *)0x437dce;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar1);
  return;
}

