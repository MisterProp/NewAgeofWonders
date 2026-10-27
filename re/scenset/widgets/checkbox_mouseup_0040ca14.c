// TAOWCheckBox.CheckMouseUp (/AoWInterface.dpl 0x0040ca14)


void AoWCheckBox_TAOWCheckBox_CheckMouseUp_23EDC2EF
               (int param_1,undefined1 param_2,undefined1 param_3,int param_4,int param_5)

{
  int iVar1;
  int iVar2;
  
                    /* 0xca14  190  AoWCheckBox.TAOWCheckBox.CheckMouseUp@23EDC2EF */
  *(undefined4 *)(param_1 + 0x6c) = 0;
  iVar1 = *(int *)(param_1 + 0x108);
  iVar2 = *(int *)(iVar1 + 0x80) + *(int *)(param_1 + 0x80);
  if ((((iVar2 + -1 < param_5) && (param_5 < iVar2 + *(int *)(param_1 + 0x78))) &&
      (*(int *)(iVar1 + 0x84) + *(int *)(param_1 + 0x84) + -1 < param_4)) &&
     (((iVar2 = *(int *)(iVar1 + 0x84) + *(int *)(param_1 + 0x84) + *(int *)(param_1 + 0x7c),
       param_4 < iVar2 && (*(char *)(param_1 + 0x2c) == '\x01')) &&
      ((*(char *)(param_1 + 0x69) == '\x01' &&
       (*(undefined4 *)(param_1 + 0x6c) = 0xffffffff, *(short *)(param_1 + 0x42) != 0)))))) {
    (**(code **)(param_1 + 0x40))
              (*(undefined4 *)(param_1 + 0x44),param_1,CONCAT31((int3)((uint)iVar1 >> 8),param_2),
               param_4,param_5,CONCAT31((int3)((uint)iVar2 >> 8),param_3));
  }
  return;
}

