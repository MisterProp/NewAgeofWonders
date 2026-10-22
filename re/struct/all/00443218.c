
void FUN_00443218(int param_1,undefined4 param_2)

{
  int *piVar1;
  undefined1 *puVar2;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_20;
  undefined1 *puStack_1c;
  undefined1 *puStack_18;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_18 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  puStack_1c = &LAB_00443309;
  uStack_20 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_20;
  FUN_004425d8(param_1);
  FUN_0044275c(param_1,param_2);
  (**(code **)(**(int **)(param_1 + 0xac) + 0x6c))(*(int **)(param_1 + 0xac),1);
  piVar1 = *(int **)(*(int *)(param_1 + 0xb0) + 0x118);
  (**(code **)(*piVar1 + 0x34))(piVar1,0);
  piVar1 = *(int **)(*(int *)(param_1 + 0xb0) + 0x118);
  (**(code **)(*piVar1 + 0x34))(piVar1,0);
  piVar1 = *(int **)(*(int *)(param_1 + 0xb0) + 0x118);
  (**(code **)(*piVar1 + 0x34))(piVar1,0);
  piVar1 = *(int **)(*(int *)(param_1 + 0xb0) + 0x118);
  (**(code **)(*piVar1 + 0x34))(piVar1,0);
  piVar1 = *(int **)(*(int *)(param_1 + 0xb0) + 0x118);
  (**(code **)(*piVar1 + 0x34))(piVar1,0);
  piVar1 = *(int **)(*(int *)(param_1 + 0xb0) + 0x118);
  (**(code **)(*piVar1 + 0x34))(piVar1,0);
  System_LoadResString_02614E1B(AoWE_StatusWarmodeRStr_04ECFCC5_exref,&local_c);
  AoWE_TranslateRStr_435799AA(local_c,&local_8);
  piVar1 = *(int **)(*(int *)(param_1 + 0xb0) + 0x118);
  (**(code **)(*piVar1 + 0x34))(piVar1,local_8);
  puVar2 = puStack_18;
  *in_FS_OFFSET = uStack_20;
  puStack_18 = &LAB_00443310;
  puStack_1c = (undefined1 *)0x443308;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar2);
  return;
}

