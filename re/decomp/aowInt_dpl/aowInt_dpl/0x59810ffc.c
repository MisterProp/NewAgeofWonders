// /aowInt.dpl : 0x59810ffc

void AoWButton_TAOWBaseButton_SetSize_23EDC2EF(int param_1)

{
  int iVar1;
  int iVar2;
  
                    /* 0x10ffc  270  AoWButton.TAOWBaseButton.SetSize@23EDC2EF */
  if (*(char *)(param_1 + 0x118) == '\0') {
    iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                      (*(undefined4 *)(param_1 + 0x11c),*(undefined4 *)(param_1 + 0x124));
    iVar2 = *(int *)(iVar2 + 0x20);
    iVar1 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                      (*(undefined4 *)(param_1 + 0x11c),*(int *)(param_1 + 0x124) + 2);
    if (*(int *)(param_1 + 0x7c) < iVar2 + *(int *)(iVar1 + 0x20)) {
      iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                        (*(undefined4 *)(param_1 + 0x11c),*(undefined4 *)(param_1 + 0x124));
      iVar2 = *(int *)(iVar2 + 0x20);
      iVar1 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                        (*(undefined4 *)(param_1 + 0x11c),*(int *)(param_1 + 0x124) + 2);
      *(int *)(param_1 + 0x7c) = iVar2 + *(int *)(iVar1 + 0x20);
    }
    iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                      (*(undefined4 *)(param_1 + 0x11c),*(undefined4 *)(param_1 + 0x124));
    *(undefined4 *)(param_1 + 0x80) = *(undefined4 *)(iVar2 + 0x24);
  }
  else if (*(char *)(param_1 + 0x118) == '\x01') {
    iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                      (*(undefined4 *)(param_1 + 0x120),*(undefined4 *)(param_1 + 0x124));
    *(undefined4 *)(param_1 + 0x7c) = *(undefined4 *)(iVar2 + 0x20);
    iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                      (*(undefined4 *)(param_1 + 0x120),*(undefined4 *)(param_1 + 0x124));
    *(undefined4 *)(param_1 + 0x80) = *(undefined4 *)(iVar2 + 0x24);
  }
  AoWComp_TAoWComponent_SetSize_23EDC2EF(param_1);
  return;
}

