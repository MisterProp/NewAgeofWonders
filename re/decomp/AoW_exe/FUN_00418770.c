// /AoW.exe : FUN_00418770

void FUN_00418770(int param_1)

{
  int iVar1;
  
  if (*(char *)(param_1 + 0x68) == '\0') {
    iVar1 = (**(code **)(**(int **)(param_1 + 0x7c) + 0x14))();
    if (*(int *)(param_1 + 0x78) < iVar1 * 0x14 + *(int *)(*(int *)(param_1 + 0x60) + 0x80)) {
      *(int *)(param_1 + 0x74) = *(int *)(param_1 + 0x74) + -1;
      if (*(int *)(param_1 + 0x74) < 1) {
        *(int *)(param_1 + 0x78) = *(int *)(param_1 + 0x78) + 1;
        *(undefined1 *)(*(int *)(param_1 + 0x60) + 0xa4) = 0;
        *(undefined4 *)(param_1 + 0x74) = 3;
      }
    }
    else {
      FUN_00418430(param_1);
    }
  }
  return;
}

