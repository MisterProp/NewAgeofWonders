
void FUN_00418430(int param_1)

{
  *(undefined1 *)(param_1 + 0x68) = 1;
  (**(code **)(**(int **)(param_1 + 0x48) + 0x6c))(*(int **)(param_1 + 0x48),1);
  (**(code **)(**(int **)(param_1 + 0x60) + 0x6c))(*(int **)(param_1 + 0x60),0);
  (**(code **)(**(int **)(param_1 + 0x44) + 0x6c))(*(int **)(param_1 + 0x44),1);
  return;
}

