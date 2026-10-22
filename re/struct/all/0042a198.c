
void FUN_0042a198(int param_1)

{
  int iVar1;
  
  iVar1 = (**(code **)(**(int **)(*(int *)(param_1 + 0x68) + 0x114) + 0x14))();
  if ((iVar1 != 0) && (*(int *)(*(int *)(param_1 + 0x68) + 0x120) != -1)) {
    (**(code **)(**(int **)(param_1 + 0x74) + 0x6c))(*(int **)(param_1 + 0x74),1);
    (**(code **)(**(int **)(param_1 + 0x6c) + 0x6c))(*(int **)(param_1 + 0x6c),0);
    return;
  }
  (**(code **)(**(int **)(param_1 + 0x74) + 0x6c))(*(int **)(param_1 + 0x74),0);
  (**(code **)(**(int **)(param_1 + 0x6c) + 0x6c))(*(int **)(param_1 + 0x6c),1);
  return;
}

