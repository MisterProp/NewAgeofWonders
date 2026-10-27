// TAOWCheckBox.CheckMouseDown (/AoWInterface.dpl 0x0040c8e8)


void AoWCheckBox_TAOWCheckBox_CheckMouseDown_23EDC2EF
               (int param_1,char param_2,undefined1 param_3,int param_4,int param_5)

{
  int iVar1;
  int iVar2;
  
                    /* 0xc8e8  191  AoWCheckBox.TAOWCheckBox.CheckMouseDown@23EDC2EF */
  AoWControl_TAOWControl_CheckMouseDown_23EDC2EF(param_1,param_2,param_3,param_4,param_5);
  *(undefined4 *)(param_1 + 0x6c) = 0;
  iVar1 = *(int *)(param_1 + 0x108);
  iVar2 = *(int *)(iVar1 + 0x80) + *(int *)(param_1 + 0x80);
  if ((((iVar2 + -1 < param_5) && (param_5 < iVar2 + *(int *)(param_1 + 0x78))) &&
      (*(int *)(iVar1 + 0x84) + *(int *)(param_1 + 0x84) + -1 < param_4)) &&
     (((param_4 < *(int *)(iVar1 + 0x84) + *(int *)(param_1 + 0x84) + *(int *)(param_1 + 0x7c) &&
       (*(char *)(param_1 + 0x2c) == '\x01')) && (*(char *)(param_1 + 0x69) == '\x01')))) {
    *(undefined4 *)(param_1 + 0x6c) = 0xffffffff;
    if (param_2 == '\0') {
      if (*(int *)(param_1 + 0xdc) < 1) {
        if (*(char *)(param_1 + 0xe0) == '\0') {
          *(undefined1 *)(param_1 + 0xe0) = 1;
        }
        else {
          *(undefined1 *)(param_1 + 0xe0) = 0;
        }
        if (*(short *)(param_1 + 0x62) != 0) {
          (**(code **)(param_1 + 0x60))(*(undefined4 *)(param_1 + 100),param_1);
        }
        *(undefined1 *)(param_1 + 0xd2) = 0;
      }
      else if (*(char *)(param_1 + 0xe0) == '\0') {
        *(undefined1 *)(param_1 + 0xe0) = 1;
        AoWWinControl_TAOWWindowControl_RadioChange_23EDC2EF
                  (*(undefined4 *)(param_1 + 0x108),param_1);
        if (*(short *)(param_1 + 0x62) != 0) {
          (**(code **)(param_1 + 0x60))(*(undefined4 *)(param_1 + 100),param_1);
        }
        *(undefined1 *)(param_1 + 0xd2) = 0;
      }
    }
    if (*(short *)(param_1 + 0x3a) != 0) {
      (**(code **)(param_1 + 0x38))
                (*(undefined4 *)(param_1 + 0x3c),param_1,param_2,param_4,param_5,param_3);
    }
  }
  return;
}

