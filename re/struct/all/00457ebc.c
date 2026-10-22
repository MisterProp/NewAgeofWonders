
void FUN_00457ebc(int param_1,int *param_2)

{
  short sVar1;
  
  sVar1 = *(short *)(param_1 + 0x140);
  if (sVar1 < 1) {
    return;
  }
  if (sVar1 < 0x4d) {
    if (sVar1 != 0x4c) {
      if (sVar1 < 0x20) {
        if ((sVar1 == 0x1f) || (sVar1 == 1)) {
LAB_00457f09:
          *(undefined1 *)(param_2 + 0x29) = 0;
          (**(code **)(*param_2 + 0xa0))(param_2,0xe);
          goto LAB_00457f3b;
        }
        if (sVar1 != 0x10) goto LAB_00457f3b;
      }
      else if (sVar1 != 0x2e) {
        if (sVar1 != 0x3d) goto LAB_00457f3b;
        goto LAB_00457f09;
      }
    }
  }
  else {
    if (sVar1 == 0x5b) goto LAB_00457f09;
    if (sVar1 != 0x6a) {
      if (sVar1 == 0x79) goto LAB_00457f09;
      if (sVar1 != 0x88) goto LAB_00457f3b;
    }
  }
  *(undefined1 *)(param_2 + 0x29) = 0;
  (**(code **)(*param_2 + 0xa0))(param_2,0x14);
LAB_00457f3b:
  *(short *)(param_1 + 0x140) = *(short *)(param_1 + 0x140) + -1;
  return;
}

