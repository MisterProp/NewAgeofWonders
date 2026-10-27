// TAOWCheckBox.CheckMouseMove (/AoWInterface.dpl 0x0040caa8)


void AoWCheckBox_TAOWCheckBox_CheckMouseMove_23EDC2EF
               (int param_1,undefined1 param_2,int param_3,int param_4)

{
  int iVar1;
  int iVar2;
  
                    /* 0xcaa8  189  AoWCheckBox.TAOWCheckBox.CheckMouseMove@23EDC2EF */
  AoWControl_TAOWControl_CheckMouseMove_23EDC2EF(param_1,param_2,param_3,param_4);
  *(undefined4 *)(param_1 + 0x6c) = 0;
  iVar1 = *(int *)(param_1 + 0x108);
  iVar2 = *(int *)(iVar1 + 0x80) + *(int *)(param_1 + 0x80);
  if ((((iVar2 <= param_3) && (param_3 <= iVar2 + *(int *)(param_1 + 0x78) + -1)) &&
      (*(int *)(iVar1 + 0x84) + *(int *)(param_1 + 0x84) <= param_4)) &&
     ((param_4 <= *(int *)(iVar1 + 0x84) + *(int *)(param_1 + 0x84) + *(int *)(param_1 + 0x7c) + -1
      && (*(undefined4 *)(param_1 + 0x6c) = 0xffffffff, *(short *)(param_1 + 0x4a) != 0)))) {
    (**(code **)(param_1 + 0x48))(*(undefined4 *)(param_1 + 0x4c),param_1,param_2,param_4,param_3);
  }
  return;
}

