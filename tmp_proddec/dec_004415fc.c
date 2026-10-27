
void FUN_004415fc(int param_1)

{
  undefined1 uVar1;
  
  if (*(int **)(param_1 + 0x1f8) != (int *)0x0) {
    (**(code **)(**(int **)(param_1 + 0x1f8) + 0xe8))();
  }
  if ((*(char *)(*(int *)(param_1 + 0x1d0) + 0xa4) == '\0') ||
     ((*(byte *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x40) & 1) == 0)) {
    uVar1 = 0;
  }
  else {
    uVar1 = 1;
  }
  *(undefined1 *)(*(int *)(param_1 + 0x1d0) + 0xa4) = uVar1;
  return;
}

