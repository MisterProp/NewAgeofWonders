
void FUN_0040f584(int param_1)

{
  int iVar1;
  
  AoWVScroll_TAOWScrollBar_SetFMin_23EDC2EF(*(undefined4 *)(param_1 + 0x7c),0);
  iVar1 = (**(code **)(**(int **)(param_1 + 0xe0) + 0x14))();
  AoWVScroll_TAOWScrollBar_SetFMax_23EDC2EF(*(undefined4 *)(param_1 + 0x7c),iVar1 + -1);
  AoWVScroll_TAOWScrollBar_SetFNum_23EDC2EF(*(undefined4 *)(param_1 + 0x7c),6);
  *(undefined1 *)(*(int *)(param_1 + 0x74) + 0xa4) = 0;
  FUN_0040f4bc(param_1,*(undefined4 *)(*(int *)(param_1 + 0x7c) + 0x148),
               *(undefined4 *)(param_1 + 0x78),*(undefined4 *)(param_1 + 0x70));
  FUN_0040f4bc(param_1,*(int *)(*(int *)(param_1 + 0x7c) + 0x148) + 1,
               *(undefined4 *)(param_1 + 0x80),*(undefined4 *)(param_1 + 0x88));
  FUN_0040f4bc(param_1,*(int *)(*(int *)(param_1 + 0x7c) + 0x148) + 2,
               *(undefined4 *)(param_1 + 0x84),*(undefined4 *)(param_1 + 0x8c));
  FUN_0040f4bc(param_1,*(int *)(*(int *)(param_1 + 0x7c) + 0x148) + 3,
               *(undefined4 *)(param_1 + 0x90),*(undefined4 *)(param_1 + 0x94));
  FUN_0040f4bc(param_1,*(int *)(*(int *)(param_1 + 0x7c) + 0x148) + 4,
               *(undefined4 *)(param_1 + 0xb8),*(undefined4 *)(param_1 + 0xbc));
  FUN_0040f4bc(param_1,*(int *)(*(int *)(param_1 + 0x7c) + 0x148) + 5,
               *(undefined4 *)(param_1 + 0xc0),*(undefined4 *)(param_1 + 0xc4));
  return;
}

