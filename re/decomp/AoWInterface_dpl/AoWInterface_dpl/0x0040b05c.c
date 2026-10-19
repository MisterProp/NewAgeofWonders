// /AoWInterface.dpl : 0x0040b05c

void AoWButton_TAOWButton_SetSize_23EDC2EF(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
                    /* 0xb05c  181  AoWButton.TAOWButton.SetSize@23EDC2EF */
  *(undefined1 *)(param_1 + 0x2c) = 0;
  if (*(char *)(param_1 + 0x114) == '\0') {
    if (*(int *)(param_1 + 0x118) != 0) {
      iVar1 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(int *)(param_1 + 0x118),0);
      iVar1 = *(int *)(iVar1 + 0x20);
      iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),2);
      if (*(int *)(param_1 + 0x78) < iVar1 + *(int *)(iVar2 + 0x20)) {
        iVar1 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),0);
        iVar1 = *(int *)(iVar1 + 0x20);
        iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),2);
        *(int *)(param_1 + 0x78) = iVar1 + *(int *)(iVar2 + 0x20);
      }
      iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),0);
      iVar1 = *(int *)(param_1 + 0x78);
      iVar2 = *(int *)(iVar2 + 0x20);
      iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),2);
      *(int *)(param_1 + 0xa0) = (iVar1 - iVar2) - *(int *)(iVar3 + 0x20);
      *(undefined1 *)(param_1 + 0x9c) = 0;
      while( true ) {
        iVar1 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),1);
        if (*(int *)(param_1 + 0xa0) <= *(int *)(iVar1 + 0x20) + -1) break;
        iVar1 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),1);
        *(int *)(param_1 + 0xa0) = *(int *)(param_1 + 0xa0) - *(int *)(iVar1 + 0x20);
        *(char *)(param_1 + 0x9c) = *(char *)(param_1 + 0x9c) + '\x01';
      }
      iVar1 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),0);
      *(undefined4 *)(param_1 + 0x7c) = *(undefined4 *)(iVar1 + 0x24);
      *(undefined1 *)(param_1 + 0x2c) = 1;
      return;
    }
  }
  else if (*(int *)(param_1 + 0x11c) != 0) {
    iVar1 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(int *)(param_1 + 0x11c),0);
    *(undefined4 *)(param_1 + 0x78) = *(undefined4 *)(iVar1 + 0x20);
    iVar1 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x11c),0);
    *(undefined4 *)(param_1 + 0x7c) = *(undefined4 *)(iVar1 + 0x24);
    *(undefined1 *)(param_1 + 0x2c) = 1;
  }
  return;
}

