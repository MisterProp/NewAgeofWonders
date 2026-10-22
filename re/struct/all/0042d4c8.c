
void FUN_0042d4c8(int param_1)

{
  int *piVar1;
  
  piVar1 = *(int **)(param_1 + 0x44);
  if (*(char *)((int)piVar1 + 0x75) != '\0') {
    (**(code **)(*piVar1 + 0x6c))(piVar1,0);
  }
  return;
}

