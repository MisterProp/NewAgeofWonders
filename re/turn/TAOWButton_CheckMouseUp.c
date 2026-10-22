
void AoWButton_TAOWButton_CheckMouseUp_23EDC2EF
               (int param_1,char param_2,undefined1 param_3,int param_4,int param_5)

{
  int iVar1;
  int iVar2;
  
                    /* 0xba6c  175  AoWButton.TAOWButton.CheckMouseUp@23EDC2EF */
  *(undefined4 *)(param_1 + 0x6c) = 0;
  *(undefined4 *)(param_1 + 0x110) = 0;
  iVar1 = *(int *)(param_1 + 0x108);
  iVar2 = *(int *)(iVar1 + 0x80) + *(int *)(param_1 + 0x80);
  if ((((iVar2 + -1 < param_5) && (param_5 < iVar2 + *(int *)(param_1 + 0x78))) &&
      (*(int *)(iVar1 + 0x84) + *(int *)(param_1 + 0x84) + -1 < param_4)) &&
     (param_4 < *(int *)(iVar1 + 0x84) + *(int *)(param_1 + 0x84) + *(int *)(param_1 + 0x7c))) {
    *(undefined4 *)(param_1 + 0x6c) = 0xffffffff;
    if (param_2 == '\0') {
      if ((*(char *)(param_1 + 0x68) == '\x01') && (*(short *)(param_1 + 0x126) != 0)) {
        (**(code **)(param_1 + 0x124))(*(undefined4 *)(param_1 + 0x128),param_1);
      }
      *(undefined1 *)(param_1 + 0xd2) = 0;
    }
    if (*(short *)(param_1 + 0x42) != 0) {
      (**(code **)(param_1 + 0x40))
                (*(undefined4 *)(param_1 + 0x44),param_1,param_2,param_4,param_5,param_3);
    }
  }
  *(undefined1 *)(param_1 + 0x68) = 0;
  return;
}

