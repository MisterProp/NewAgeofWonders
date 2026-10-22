
void FUN_00416d9c(int param_1)

{
  int *piVar1;
  
  FUN_00415a94(param_1);
  piVar1 = *(int **)(param_1 + 0x2d8);
  *(undefined1 *)(piVar1 + 7) = 2;
  *(undefined4 *)(param_1 + 0x2d8) = 0;
  (**(code **)(**(int **)(param_1 + 0x2cc) + 0x6c))(*(int **)(param_1 + 0x2cc),0);
  FUN_0041468c(*(undefined4 *)PTR_DAT_0045a454,*(undefined4 *)(piVar1[5] + 0x60));
  (**(code **)(*piVar1 + 0x4c))();
  (**(code **)(*piVar1 + 0x2c))();
  return;
}

