// /aowInt.dpl : 0x59811120

void AoWButton_TAOWBaseButton_SetILButton_23EDC2EF(int param_1,int param_2)

{
                    /* 0x11120  268  AoWButton.TAOWBaseButton.SetILButton@23EDC2EF */
  if (param_2 != *(int *)(param_1 + 0x11c)) {
    *(undefined4 *)(param_1 + 0x120) = 0;
    *(undefined1 *)(param_1 + 0x2c) = 0;
    *(int *)(param_1 + 0x11c) = param_2;
  }
  return;
}

