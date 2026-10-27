// /AoW.exe : 00441550

void DlgKeyDown(int param_1,undefined4 param_2,undefined2 *param_3)

{
  switch(*param_3) {
  case 0x1b:
    (**(code **)(**(int **)(param_1 + 0x44) + 0x6c))(*(int **)(param_1 + 0x44),0);
    break;
  case 0x25:
    *(int *)(param_1 + 0x1e0) = *(int *)(param_1 + 0x1e0) + -0x10;
    *(undefined1 *)(*(int *)(param_1 + 0x1d0) + 0xa4) = 0;
    break;
  case 0x26:
    *(int *)(param_1 + 0x1e4) = *(int *)(param_1 + 0x1e4) + -0x10;
    *(undefined1 *)(*(int *)(param_1 + 0x1d0) + 0xa4) = 0;
    break;
  case 0x27:
    *(int *)(param_1 + 0x1e0) = *(int *)(param_1 + 0x1e0) + 0x10;
    *(undefined1 *)(*(int *)(param_1 + 0x1d0) + 0xa4) = 0;
    break;
  case 0x28:
    *(int *)(param_1 + 0x1e4) = *(int *)(param_1 + 0x1e4) + 0x10;
    *(undefined1 *)(*(int *)(param_1 + 0x1d0) + 0xa4) = 0;
  }
  return;
}

