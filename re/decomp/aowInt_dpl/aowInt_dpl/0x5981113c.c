// /aowInt.dpl : 0x5981113c

void AoWButton_TAOWBaseButton_SetILIcon_23EDC2EF(int param_1,int param_2)

{
                    /* 0x1113c  267  AoWButton.TAOWBaseButton.SetILIcon@23EDC2EF */
  if (param_2 != *(int *)(param_1 + 0x120)) {
    *(undefined4 *)(param_1 + 0x11c) = 0;
    *(undefined1 *)(param_1 + 0x2c) = 0;
    *(int *)(param_1 + 0x120) = param_2;
  }
  return;
}

