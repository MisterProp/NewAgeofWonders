
void FUN_00442b20(int param_1,int param_2)

{
  int *piVar1;
  
  FUN_004425d8(param_1);
  FUN_0044275c(param_1,param_2);
  *(int *)(*(int *)PTR_DAT_0045a420 + 0xbc) = param_2;
  (**(code **)(**(int **)(param_1 + 0x74) + 0x6c))(*(int **)(param_1 + 0x74),1);
  piVar1 = *(int **)(*(int *)(param_1 + 0x78) + 0x118);
  (**(code **)(*piVar1 + 8))(piVar1,*(undefined4 *)(param_2 + 0x30));
  return;
}

