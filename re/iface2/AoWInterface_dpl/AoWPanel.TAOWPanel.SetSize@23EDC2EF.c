// /AoWInterface.dpl : AoWPanel.TAOWPanel.SetSize@23EDC2EF

void AoWPanel_TAOWPanel_SetSize_23EDC2EF(int *param_1)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  
                    /* 0x1f4c0  432  AoWPanel.TAOWPanel.SetSize@23EDC2EF */
  if (((*(char *)((int)param_1 + 0x69) == '\0') && (param_1[0x75] != 0)) &&
     (*(int *)(param_1[0x75] + 0x114) == param_1[0x39])) {
    *(undefined1 *)((int)param_1 + 0x69) = 1;
  }
  if ((*(char *)((int)param_1 + 0x69) != '\0') || ((char)param_1[0xb] == '\0')) {
    *(undefined1 *)(param_1 + 0xb) = 0;
    if (*(byte *)(param_1 + 0x74) < 2) {
      if ((param_1[0x72] != 0) && ((param_1[0x73] != 0 || ((char)param_1[0x74] == '\x01')))) {
        iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1[0x72],0);
        iVar4 = *(int *)(iVar4 + 0x20);
        iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1[0x72],2);
        if (param_1[0x1e] < iVar4 + *(int *)(iVar3 + 0x20)) {
          iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1[0x72],0);
          iVar4 = *(int *)(iVar4 + 0x20);
          iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1[0x72],2);
          param_1[0x1e] = iVar4 + *(int *)(iVar3 + 0x20);
        }
        iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1[0x72],0);
        iVar4 = *(int *)(iVar4 + 0x24);
        iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1[0x72],5);
        if (param_1[0x1f] < iVar4 + *(int *)(iVar3 + 0x24)) {
          iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1[0x72],0);
          iVar4 = *(int *)(iVar4 + 0x24);
          iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1[0x72],5);
          param_1[0x1f] = iVar4 + *(int *)(iVar3 + 0x24);
        }
        if (param_1[0x76] != 0) {
          iVar4 = *(int *)(param_1[0x76] + 0x78);
          if (iVar4 < param_1[0x20] + param_1[0x1e]) {
            param_1[0x20] = iVar4 - param_1[0x1e];
          }
          if (param_1[0x20] < 0) {
            param_1[0x20] = 0;
            if (*(int *)(param_1[0x76] + 0x78) < param_1[0x1e]) {
              param_1[0x1e] = *(int *)(param_1[0x76] + 0x78);
            }
          }
          if (*(int *)(param_1[0x76] + 0x7c) < param_1[0x21] + param_1[0x1f]) {
            param_1[0x21] = *(int *)(param_1[0x76] + 0x7c) - param_1[0x1f];
          }
          if (param_1[0x21] < 0) {
            param_1[0x21] = 0;
            if (*(int *)(param_1[0x76] + 0x7c) < param_1[0x1f]) {
              param_1[0x1f] = *(int *)(param_1[0x76] + 0x7c);
            }
          }
        }
        param_1[0x28] = param_1[0x1e];
        param_1[0x29] = param_1[0x1f];
        if (*(char *)((int)param_1 + 0x1d1) == '\0') {
          iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1[0x72],0);
          iVar4 = param_1[0x1e];
          iVar3 = *(int *)(iVar3 + 0x20);
          iVar1 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1[0x72],2);
          param_1[0x28] = (iVar4 - iVar3) - *(int *)(iVar1 + 0x20);
        }
        else {
          param_1[0x28] = param_1[0x1e];
        }
        iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1[0x72],0);
        iVar4 = param_1[0x1f];
        iVar3 = *(int *)(iVar3 + 0x24);
        iVar1 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1[0x72],5);
        param_1[0x29] = (iVar4 - iVar3) - *(int *)(iVar1 + 0x24);
        *(undefined1 *)(param_1 + 0x27) = 0;
        *(undefined1 *)((int)param_1 + 0x9d) = 0;
        while (iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1[0x72],1),
              *(int *)(iVar4 + 0x20) + -1 < param_1[0x28]) {
          iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1[0x72],1);
          param_1[0x28] = param_1[0x28] - *(int *)(iVar4 + 0x20);
          *(char *)(param_1 + 0x27) = (char)param_1[0x27] + '\x01';
        }
        while (iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1[0x72],3),
              *(int *)(iVar4 + 0x24) + -1 < param_1[0x29]) {
          iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1[0x72],3);
          param_1[0x29] = param_1[0x29] - *(int *)(iVar4 + 0x24);
          *(char *)((int)param_1 + 0x9d) = *(char *)((int)param_1 + 0x9d) + '\x01';
        }
        *(undefined1 *)(param_1 + 0xb) = 1;
      }
    }
    else if (((byte)(*(byte *)(param_1 + 0x74) - 2) < 2) &&
            ((param_1[0x73] != 0 || ((char)param_1[0x74] == '\x03')))) {
      if (param_1[0x76] != 0) {
        iVar4 = *(int *)(param_1[0x76] + 0x78);
        if (iVar4 < param_1[0x20] + param_1[0x1e]) {
          param_1[0x20] = iVar4 - param_1[0x1e];
        }
        if (param_1[0x20] < 0) {
          param_1[0x20] = 0;
          if (*(int *)(param_1[0x76] + 0x78) < param_1[0x1e]) {
            param_1[0x1e] = *(int *)(param_1[0x76] + 0x78);
          }
        }
        if (*(int *)(param_1[0x76] + 0x7c) < param_1[0x21] + param_1[0x1f]) {
          param_1[0x21] = *(int *)(param_1[0x76] + 0x7c) - param_1[0x1f];
        }
        if (param_1[0x21] < 0) {
          param_1[0x21] = 0;
          if (*(int *)(param_1[0x76] + 0x7c) < param_1[0x1f]) {
            param_1[0x1f] = *(int *)(param_1[0x76] + 0x7c);
          }
        }
      }
      *(undefined1 *)(param_1 + 0xb) = 1;
    }
    if (*(char *)((int)param_1 + 0x1d2) == '\0') {
      param_1[0x6e] = param_1[0x20];
      param_1[0x6f] = param_1[0x21];
      if (param_1[0x76] != 0) {
        param_1[0x20] = param_1[0x20] + *(int *)(param_1[0x76] + 0x80);
        param_1[0x21] = param_1[0x21] + *(int *)(param_1[0x76] + 0x84);
      }
      iVar4 = *(int *)(param_1[0x65] + 8);
      if (-1 < iVar4 + -1) {
        iVar3 = 0;
        do {
          piVar2 = (int *)Classes_TList_Get_23EDC2EF(param_1[0x65],iVar3);
          (**(code **)(*param_1 + 0x58))(param_1,piVar2);
          (**(code **)(*piVar2 + 0x50))();
          iVar3 = iVar3 + 1;
          iVar4 = iVar4 + -1;
        } while (iVar4 != 0);
      }
      param_1[0x20] = param_1[0x6e];
      param_1[0x21] = param_1[0x6f];
    }
    if (*(int *)(param_1[0x76] + 0x1a4) != 0) {
      param_1[0x69] = *(int *)(param_1[0x76] + 0x1a4);
    }
    *(undefined1 *)((int)param_1 + 0xd2) = 0;
    *(undefined1 *)(param_1 + 0x71) = 0;
  }
  return;
}

