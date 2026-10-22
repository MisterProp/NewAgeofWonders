
void FUN_00437080(int param_1,int *param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5)

{
  *(undefined4 *)(param_1 + 0x78) = param_4;
  *(undefined4 *)(param_1 + 0x7c) = param_5;
  *(int **)(param_1 + 0x80) = param_2;
  (**(code **)(*param_2 + 0x28))();
  AoWLabel_TAOWLabel_SetGText_23EDC2EF
            (*(undefined4 *)(param_1 + 100),*(undefined4 *)(*(int *)(param_1 + 0x80) + 0x24));
  (**(code **)(**(int **)(param_1 + 0x44) + 0x6c))(*(int **)(param_1 + 0x44),1);
  return;
}

