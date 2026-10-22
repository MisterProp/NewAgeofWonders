
void FUN_0042e728(int param_1)

{
  if (*(int **)(param_1 + 0x230) != (int *)0x0) {
    (**(code **)(**(int **)(param_1 + 0x230) + 0x2c))();
    *(undefined4 *)(param_1 + 0x230) = 0;
  }
  System_TObject_Free_23EDC2EF(*(undefined4 *)(param_1 + 0x244));
  System_TObject_Free_23EDC2EF(*(undefined4 *)(param_1 + 0x224));
  return;
}

