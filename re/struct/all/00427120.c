
void FUN_00427120(int param_1)

{
  *(undefined1 *)(*(int *)(param_1 + 0x54) + 0xa4) = 0;
  if (*(int *)(*(int *)(param_1 + 0x180) + 0x120) != -1) {
    (**(code **)(**(int **)(param_1 + 0x17c) + 0xa4))(*(int **)(param_1 + 0x17c),0x25);
    (**(code **)(**(int **)(param_1 + 0x17c) + 0xa8))(*(int **)(param_1 + 0x17c),0x28);
    (**(code **)(**(int **)(param_1 + 0x17c) + 0xa0))(*(int **)(param_1 + 0x17c),0x22);
    return;
  }
  (**(code **)(**(int **)(param_1 + 0x17c) + 0xa4))(*(int **)(param_1 + 0x17c),0x2e);
  (**(code **)(**(int **)(param_1 + 0x17c) + 0xa8))(*(int **)(param_1 + 0x17c),0x31);
  (**(code **)(**(int **)(param_1 + 0x17c) + 0xa0))(*(int **)(param_1 + 0x17c),0x2b);
  return;
}

