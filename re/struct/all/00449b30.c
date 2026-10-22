
void FUN_00449b30(int param_1)

{
  if (*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5) != -1) {
    switch(*(undefined4 *)(*(int *)(param_1 + 0x50) + 0x118)) {
    case 0:
      FUN_00448eb4();
      break;
    case 1:
      FUN_00449230();
      break;
    case 2:
      FUN_0044975c();
      break;
    case 3:
      FUN_00449894();
      break;
    case 4:
      FUN_00449894();
      break;
    case 5:
      FUN_00449b2c();
    }
    return;
  }
  (**(code **)(**(int **)(param_1 + 0x44) + 0x6c))(*(int **)(param_1 + 0x44),0);
  return;
}

