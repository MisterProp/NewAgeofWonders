
void FUN_0044e424(int param_1,int *param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5)

{
  int *piVar1;
  
  AoWVScroll_TAOWScrollBar_SetFPos_23EDC2EF(*(undefined4 *)(param_1 + 0x54),0);
  FUN_0044e2bc(param_1);
  *(int **)(param_1 + 100) = param_2;
  (**(code **)(*param_2 + 0x28))();
  *(undefined4 *)(param_1 + 0x5c) = param_4;
  *(undefined4 *)(param_1 + 0x60) = param_5;
  (**(code **)(**(int **)(*(int *)(param_1 + 0x50) + 0x118) + 0x40))();
  piVar1 = *(int **)(*(int *)(param_1 + 0x50) + 0x118);
  (**(code **)(*piVar1 + 0x34))(piVar1,param_2[7]);
  (**(code **)(**(int **)(*(int *)(param_1 + 0x48) + 0x118) + 0x40))();
  piVar1 = *(int **)(*(int *)(param_1 + 0x48) + 0x118);
  (**(code **)(*piVar1 + 0x3c))(piVar1,param_2[6]);
  (**(code **)(**(int **)(param_1 + 0x44) + 0x6c))(*(int **)(param_1 + 0x44),1);
  return;
}

