// /AoW.exe : FUN_00409c68

void FUN_00409c68(int param_1)

{
  int iVar1;
  
  if (*(char *)(*(int *)(param_1 + 0x44) + 0x75) != '\0') {
    *(int *)(param_1 + 0x21c) = *(int *)(param_1 + 0x21c) + *(int *)(param_1 + 0x224);
    if ((100 < *(int *)(param_1 + 0x21c)) || (*(int *)(param_1 + 0x21c) < 0x32)) {
      iVar1 = *(int *)(param_1 + 0x224);
      *(int *)(param_1 + 0x224) = -iVar1;
      *(int *)(param_1 + 0x21c) = *(int *)(param_1 + 0x21c) + -iVar1;
    }
    switch(*(undefined1 *)(param_1 + 0x220)) {
    case 0:
      *(undefined1 *)(*(int *)(param_1 + 0x128) + 0xa4) = 0;
      break;
    case 1:
      *(undefined1 *)(*(int *)(param_1 + 300) + 0xa4) = 0;
      break;
    case 2:
      *(undefined1 *)(*(int *)(param_1 + 0x130) + 0xa4) = 0;
      break;
    case 3:
      *(undefined1 *)(*(int *)(param_1 + 0x134) + 0xa4) = 0;
      break;
    case 4:
      *(undefined1 *)(*(int *)(param_1 + 0x138) + 0xa4) = 0;
      *(undefined1 *)(*(int *)(param_1 + 0x13c) + 0xa4) = 0;
    }
    if ((DAT_0045b074 != 0) &&
       (*(int *)(param_1 + 0x22c) = *(int *)(param_1 + 0x22c) + -1, *(int *)(param_1 + 0x22c) < 0))
    {
      *(undefined4 *)(param_1 + 0x22c) = 2;
      *(byte *)(param_1 + 0x228) = *(byte *)(param_1 + 0x228) ^ 1;
    }
  }
  return;
}

