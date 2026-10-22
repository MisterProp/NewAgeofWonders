
void FUN_00438848(int param_1,int *param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5)

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
  puStack_1c = &LAB_0043891e;
  uStack_20 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_20;
  FUN_00438604(param_1);
  *(int **)(param_1 + 0x9c) = param_2;
  (**(code **)(*param_2 + 0x28))();
  *(undefined4 *)(param_1 + 0x94) = param_4;
  *(undefined4 *)(param_1 + 0x98) = param_5;
  System_LoadResString_02614E1B(AoWE_AcceptRStr_04ECFCC5_exref,&local_c);
  AoWE_TranslateRStr_435799AA(local_c,&local_8);
  AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x74),local_8);
  System_LoadResString_02614E1B(AoWE_AttackRStr_04ECFCC5_exref,&local_c);
  AoWE_TranslateRStr_435799AA(local_c,&local_8);
  AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x68),local_8);
  System_LoadResString_02614E1B(AoWE_CancelRStr_8276FCC3_exref,&local_c);
  AoWE_TranslateRStr_435799AA(local_c,&local_8);
  AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x70),local_8);
  FUN_0043864c(param_1);
  (**(code **)(**(int **)(param_1 + 0x78) + 0x6c))(*(int **)(param_1 + 0x78),1);
  puVar1 = puStack_18;
  *in_FS_OFFSET = uStack_20;
  puStack_18 = &LAB_00438925;
  puStack_1c = (undefined1 *)0x43891d;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar1);
  return;
}

