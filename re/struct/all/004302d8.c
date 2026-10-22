
void FUN_004302d8(int param_1)

{
  if (*(int **)(param_1 + 0x230) != (int *)0x0) {
    (**(code **)(**(int **)(param_1 + 0x230) + 0x2c))();
    *(undefined4 *)(param_1 + 0x230) = 0;
  }
  AoWE_TSpellList_Clear_23EDC2EF(*(undefined4 *)(param_1 + 0x224));
  *(undefined4 *)(param_1 + 0x228) = 0;
  *(undefined4 *)(param_1 + 0x22c) = 0;
  (**(code **)(**(int **)(param_1 + 0x1cc) + 0x6c))(*(int **)(param_1 + 0x1cc),1);
  (**(code **)(**(int **)(param_1 + 0x15c) + 0x6c))(*(int **)(param_1 + 0x15c),1);
  (**(code **)(**(int **)(param_1 + 0x124) + 0x6c))(*(int **)(param_1 + 0x124),1);
  (**(code **)(**(int **)(param_1 + 0x124) + 0xa4))(*(int **)(param_1 + 0x124),0x11);
  (**(code **)(**(int **)(param_1 + 0x124) + 0xa8))(*(int **)(param_1 + 0x124),0x12);
  (**(code **)(**(int **)(param_1 + 0x124) + 0xa0))(*(int **)(param_1 + 0x124),0x10);
  (**(code **)(**(int **)(param_1 + 300) + 0x6c))(*(int **)(param_1 + 300),1);
  (**(code **)(**(int **)(param_1 + 0x128) + 0x6c))(*(int **)(param_1 + 0x128),1);
  (**(code **)(**(int **)(param_1 + 0x130) + 0x6c))(*(int **)(param_1 + 0x130),1);
  (**(code **)(**(int **)(param_1 + 0x118) + 0x6c))(*(int **)(param_1 + 0x118),1);
  (**(code **)(**(int **)(param_1 + 0x120) + 0x6c))(*(int **)(param_1 + 0x120),1);
  (**(code **)(**(int **)(param_1 + 0x11c) + 0x6c))(*(int **)(param_1 + 0x11c),1);
  return;
}

