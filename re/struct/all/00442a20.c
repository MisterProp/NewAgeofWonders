
void FUN_00442a20(int param_1,int *param_2)

{
  int *piVar1;
  undefined1 *puVar2;
  undefined4 uVar3;
  int *in_FS_OFFSET;
  int iStack_2c;
  undefined1 *puStack_28;
  undefined1 *puStack_24;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_24 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  local_10 = 0;
  local_14 = 0;
  local_18 = 0;
  puStack_28 = &LAB_00442b11;
  iStack_2c = *in_FS_OFFSET;
  *in_FS_OFFSET = (int)&iStack_2c;
  FUN_004425d8(param_1);
  FUN_0044275c(param_1,param_2);
  (**(code **)(**(int **)(param_1 + 0xac) + 0x6c))(*(int **)(param_1 + 0xac),1);
  uVar3 = (**(code **)(*param_2 + 0x144))();
  piVar1 = *(int **)(*(int *)(param_1 + 0xb0) + 0x118);
  (**(code **)(*piVar1 + 8))(piVar1,uVar3);
  piVar1 = *(int **)(*(int *)(param_1 + 0xb0) + 0x118);
  (**(code **)(*piVar1 + 0x34))(piVar1,0);
  SysUtils_IntToStr_0F6FDFF6((char)param_2[0xf],&local_14);
  local_10 = local_14;
  local_c = CONCAT31(local_c._1_3_,0xb);
  System_LoadResString_02614E1B(AoWE_IncomeXRStr_04ECFCC5_exref,&local_8);
  AoWE_TranslateRStr_435799AA(local_8,&local_18);
  AoWE_Format_D8F48273(local_18,&local_10,0);
  piVar1 = *(int **)(*(int *)(param_1 + 0xb0) + 0x118);
  (**(code **)(*piVar1 + 0x34))(piVar1,local_8);
  (**(code **)(**(int **)(param_1 + 0x10c) + 0x6c))(*(int **)(param_1 + 0x10c),1);
  puVar2 = puStack_28;
  *in_FS_OFFSET = (int)&local_8;
  puStack_28 = &LAB_00442b18;
  iStack_2c = 0x442b08;
  System__LStrArrayClr_51F89FF7(&local_18,2,puVar2);
  iStack_2c = 0x442b10;
  System__LStrClr_40929B27(&local_8);
  return;
}

