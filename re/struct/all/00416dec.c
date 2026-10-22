
void FUN_00416dec(int param_1)

{
  int *piVar1;
  
  FUN_00415a94(param_1);
  piVar1 = *(int **)(param_1 + 0x2d8);
  *(undefined1 *)(piVar1 + 7) = 3;
  *(undefined4 *)(param_1 + 0x2d8) = 0;
  (**(code **)(**(int **)(param_1 + 0x2cc) + 0x6c))(*(int **)(param_1 + 0x2cc),0);
  (**(code **)(*piVar1 + 0x4c))();
  (**(code **)(*piVar1 + 0x2c))();
  return;
}

