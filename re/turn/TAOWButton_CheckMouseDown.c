
void AoWButton_TAOWButton_CheckMouseDown_23EDC2EF
               (int param_1,char param_2,undefined1 param_3,int param_4,int param_5)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
                    /* 0xb9bc  176  AoWButton.TAOWButton.CheckMouseDown@23EDC2EF */
  AoWControl_TAOWControl_CheckMouseDown_23EDC2EF(param_1,param_2,param_3,param_4,param_5);
  *(undefined4 *)(param_1 + 0x6c) = 0;
  iVar1 = *(int *)(param_1 + 0x108);
  iVar3 = *(int *)(iVar1 + 0x80) + *(int *)(param_1 + 0x80);
  iVar2 = iVar3 + -1;
  if ((((iVar2 < param_5) && (param_5 < iVar3 + *(int *)(param_1 + 0x78))) &&
      (*(int *)(iVar1 + 0x84) + *(int *)(param_1 + 0x84) + -1 < param_4)) &&
     (iVar1 = *(int *)(iVar1 + 0x84) + *(int *)(param_1 + 0x84) + *(int *)(param_1 + 0x7c),
     param_4 < iVar1)) {
    *(undefined4 *)(param_1 + 0x6c) = 0xffffffff;
    if (param_2 == '\0') {
      *(undefined1 *)(param_1 + 0x68) = 1;
      *(undefined4 *)(param_1 + 0x110) = 0xffffffff;
      *(undefined1 *)(param_1 + 0xd2) = 0;
    }
    if (*(short *)(param_1 + 0x3a) != 0) {
      (**(code **)(param_1 + 0x38))
                (*(undefined4 *)(param_1 + 0x3c),param_1,CONCAT31((int3)((uint)iVar2 >> 8),param_2),
                 param_4,param_5,CONCAT31((int3)((uint)iVar1 >> 8),param_3));
    }
  }
  return;
}

