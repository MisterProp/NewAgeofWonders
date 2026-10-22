
int ImageLib_TImageSequence_GetFrameIndex_23EDC2EF(int param_1,int param_2,int param_3)

{
  uint uVar1;
  undefined4 uVar2;
  int iVar3;
  uint uVar4;
  
                    /* 0x186ec  353  ImageLib.TImageSequence.GetFrameIndex@23EDC2EF */
  if (param_2 < 0) {
    iVar3 = -1;
  }
  else if (*(byte *)(param_1 + 0x19) == 0) {
    iVar3 = *(int *)(param_1 + 0x20);
  }
  else {
    switch(*(byte *)(param_1 + 0x19) & 0x7f) {
    default:
      iVar3 = -1;
      break;
    case 1:
      if (param_2 != *(int *)(param_1 + 0x2c)) {
        if (*(char *)(param_1 + 0x30) == '\0') {
          *(uint *)(param_1 + 0x34) = *(int *)(param_1 + 0x34) + 1U & 0xffffff;
          *(undefined1 *)(param_1 + 0x30) = *(undefined1 *)(param_1 + 0x18);
        }
        else {
          *(char *)(param_1 + 0x30) = *(char *)(param_1 + 0x30) + -1;
        }
        *(int *)(param_1 + 0x2c) = param_2;
      }
      iVar3 = (*(int *)(param_1 + 0x34) + param_3) % *(int *)(*(int *)(param_1 + 0x10) + 0x44);
      break;
    case 2:
      if (param_2 != *(int *)(param_1 + 0x2c)) {
        if (*(char *)(param_1 + 0x30) == '\0') {
          *(uint *)(param_1 + 0x34) = *(int *)(param_1 + 0x34) + 1U & 0xffffff;
          *(undefined1 *)(param_1 + 0x30) = *(undefined1 *)(param_1 + 0x18);
        }
        else {
          *(char *)(param_1 + 0x30) = *(char *)(param_1 + 0x30) + -1;
        }
        *(int *)(param_1 + 0x2c) = param_2;
      }
      iVar3 = *(int *)(*(int *)(param_1 + 0x10) + 0x44);
      uVar1 = (*(int *)(param_1 + 0x34) + param_3) % ((iVar3 + -1) * 2) - (iVar3 + -2);
      uVar4 = (int)uVar1 >> 0x1f;
      iVar3 = (uVar1 ^ uVar4) - uVar4;
      break;
    case 3:
      if ((param_2 != *(int *)(param_1 + 0x2c)) &&
         (param_2 % (int)(*(byte *)(param_1 + 0x18) + 1) == 0)) {
        uVar2 = System__RandInt_51F89FF7(*(undefined4 *)(*(int *)(param_1 + 0x10) + 0x44));
        *(undefined4 *)(param_1 + 0x20) = uVar2;
        *(int *)(param_1 + 0x2c) = param_2;
      }
      iVar3 = (*(int *)(param_1 + 0x20) + param_3) % *(int *)(*(int *)(param_1 + 0x10) + 0x44);
      break;
    case 4:
      if (param_2 != *(int *)(param_1 + 0x2c)) {
        if (*(char *)(param_1 + 0x30) == '\0') {
          *(uint *)(param_1 + 0x34) = *(int *)(param_1 + 0x34) + 1U & 0xffffff;
          *(undefined1 *)(param_1 + 0x30) = *(undefined1 *)(param_1 + 0x18);
        }
        else {
          *(char *)(param_1 + 0x30) = *(char *)(param_1 + 0x30) + -1;
        }
        *(int *)(param_1 + 0x2c) = param_2;
      }
      iVar3 = *(int *)(*(int *)(param_1 + 0x1c) + 8);
      if (iVar3 == 0) {
        iVar3 = -1;
      }
      else {
        iVar3 = *(int *)(*(int *)(*(int *)(param_1 + 0x1c) + 4) +
                        ((*(int *)(param_1 + 0x34) + param_3) % iVar3) * 4);
      }
      break;
    case 5:
      if ((param_2 != *(int *)(param_1 + 0x2c)) &&
         (param_2 % (int)(*(byte *)(param_1 + 0x18) + 1) == 0)) {
        uVar2 = System__RandInt_51F89FF7(*(undefined4 *)(*(int *)(param_1 + 0x1c) + 8));
        *(undefined4 *)(param_1 + 0x20) = uVar2;
        *(int *)(param_1 + 0x2c) = param_2;
      }
      iVar3 = *(int *)(*(int *)(param_1 + 0x1c) + 8);
      if (iVar3 == 0) {
        iVar3 = -1;
      }
      else {
        iVar3 = *(int *)(*(int *)(*(int *)(param_1 + 0x1c) + 4) +
                        ((*(int *)(param_1 + 0x20) + param_3) % iVar3) * 4);
      }
    }
  }
  return iVar3;
}

