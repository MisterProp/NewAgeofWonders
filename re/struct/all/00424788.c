
void FUN_00424788(int param_1,int *param_2)

{
  int *piVar1;
  
  piVar1 = *(int **)(param_1 + 0x27c);
  if (param_2 != piVar1) {
    if (piVar1 != (int *)0x0) {
      (**(code **)(*piVar1 + 0x2c))();
    }
    *(int **)(param_1 + 0x27c) = param_2;
    if (param_2 != (int *)0x0) {
      (**(code **)(*param_2 + 0x28))();
    }
  }
  return;
}

