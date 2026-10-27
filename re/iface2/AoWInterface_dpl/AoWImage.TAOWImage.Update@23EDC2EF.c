// /AoWInterface.dpl : AoWImage.TAOWImage.Update@23EDC2EF

void AoWImage_TAOWImage_Update_23EDC2EF(int param_1)

{
                    /* 0x11ab8  266  AoWImage.TAOWImage.Update@23EDC2EF */
  AoWControl_TAOWControl_Update_23EDC2EF(param_1);
  if (*(char *)(param_1 + 0x110) == '\x01') {
    if (*(int *)(param_1 + 0x118) < 1) {
      *(undefined4 *)(param_1 + 0x118) = *(undefined4 *)(param_1 + 0x114);
      *(int *)(param_1 + 0x11c) = *(int *)(param_1 + 0x11c) + 1;
      if (*(int *)(*(int *)(param_1 + 0x70) + 0x44) + -1 < *(int *)(param_1 + 0x11c)) {
        *(undefined4 *)(param_1 + 0x11c) = 0;
      }
    }
    else {
      *(int *)(param_1 + 0x118) = *(int *)(param_1 + 0x118) + -1;
    }
    *(undefined1 *)(param_1 + 0xd2) = 0;
  }
  return;
}

