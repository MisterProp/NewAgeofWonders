// /AoW.exe : FUN_0043f534

undefined4 FUN_0043f534(int param_1,int param_2)

{
  char cVar1;
  undefined4 uVar2;
  
  if (*(int *)(param_2 + 8) == 1) {
    return 4;
  }
  if (*(int *)(param_2 + 8) == 10) {
    cVar1 = (**(code **)(**(int **)(param_1 + 0x1e8) + 0x230))();
    if (cVar1 == '\b') {
      return 0x2f;
    }
    uVar2 = 0x30;
  }
  else {
    uVar2 = 4;
  }
  return uVar2;
}

