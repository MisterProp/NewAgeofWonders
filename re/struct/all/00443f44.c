
void FUN_00443f44(int param_1,int *param_2)

{
  int *piVar1;
  undefined4 uVar2;
  
  FUN_004425d8(param_1);
  FUN_0044275c(param_1,param_2);
  (**(code **)(**(int **)(param_1 + 0xac) + 0x6c))(*(int **)(param_1 + 0xac),1);
  uVar2 = (**(code **)(*param_2 + 0x144))();
  piVar1 = *(int **)(*(int *)(param_1 + 0xb0) + 0x118);
  (**(code **)(*piVar1 + 8))(piVar1,uVar2);
  return;
}

