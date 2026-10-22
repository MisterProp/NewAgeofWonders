
void FUN_00418458(int param_1)

{
  (**(code **)(**(int **)(param_1 + 0x44) + 0x6c))(*(int **)(param_1 + 0x44),1);
  (**(code **)(**(int **)(param_1 + 0x48) + 0x6c))(*(int **)(param_1 + 0x48),0);
  (**(code **)(**(int **)(*(int *)PTR_DAT_0045a28c + 0x44) + 0x6c))
            (*(int **)(*(int *)PTR_DAT_0045a28c + 0x44),1);
  (**(code **)(**(int **)(param_1 + 0x60) + 0x6c))(*(int **)(param_1 + 0x60),1);
  *(undefined4 *)(param_1 + 0x74) = 0;
  *(undefined4 *)(param_1 + 0x78) = 0;
  (**(code **)(**(int **)(param_1 + 0x7c) + 0x40))();
  return;
}

