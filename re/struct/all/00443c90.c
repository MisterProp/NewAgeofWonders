
void FUN_00443c90(int param_1,int *param_2)

{
  int *piVar1;
  undefined1 *puVar2;
  char cVar3;
  int *in_FS_OFFSET;
  undefined4 *puVar4;
  undefined4 *puVar5;
  undefined1 *puStack_30;
  undefined1 *puStack_2c;
  undefined1 *puStack_28;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_28 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  local_10 = 0;
  local_14 = 0;
  local_18 = 0;
  local_1c = 0;
  puStack_2c = &LAB_00443dc4;
  puStack_30 = (undefined1 *)*in_FS_OFFSET;
  *in_FS_OFFSET = (int)&puStack_30;
  FUN_004425d8(param_1);
  FUN_0044275c(param_1,param_2);
  (**(code **)(**(int **)(param_1 + 0xac) + 0x6c))(*(int **)(param_1 + 0xac),1);
  cVar3 = System__IsClass_51F89FF7(param_2,City__TCity_01CDE5D7_exref);
  if (cVar3 == '\0') {
    puVar5 = (undefined4 *)0x443d2e;
    (**(code **)(*param_2 + 0x148))(param_2,&local_8);
  }
  else {
    puVar5 = &local_8;
    (**(code **)(*param_2 + 0x148))(param_2,&local_14,*param_2,puVar5);
    local_10 = local_14;
    local_c = CONCAT31(local_c._1_3_,0xb);
    puVar4 = &local_10;
    System_LoadResString_02614E1B(AoWE_CityOfXRStr_04ECFCC5_exref,&local_1c);
    AoWE_TranslateRStr_435799AA(local_1c,&local_18);
    AoWE_Format_D8F48273(local_18,puVar4,0);
  }
  puVar4 = &local_14;
  System_LoadResString_02614E1B(AoWE_TheXliesInRuinsRStr_04ECFCC5_exref,&local_14);
  AoWE_TranslateRStr_435799AA(local_14,&local_18);
  local_10 = local_8;
  local_c = CONCAT31(local_c._1_3_,0xb);
  AoWE_Format_D8F48273(local_18,&local_10,0,puVar4,puVar5);
  piVar1 = *(int **)(*(int *)(param_1 + 0xb0) + 0x118);
  (**(code **)(*piVar1 + 0x2c))(piVar1,local_14);
  piVar1 = *(int **)(*(int *)(param_1 + 0xb0) + 0x118);
  (**(code **)(*piVar1 + 0x54))(piVar1,0,0);
  piVar1 = *(int **)(*(int *)(param_1 + 0xb0) + 0x118);
  (**(code **)(*piVar1 + 0x54))(piVar1,0,0);
  puVar2 = puStack_30;
  *in_FS_OFFSET = (int)puVar4;
  puStack_30 = &LAB_00443dcb;
  System__LStrArrayClr_51F89FF7(&local_1c,3,puVar2);
  System__LStrClr_40929B27(&local_8);
  return;
}

