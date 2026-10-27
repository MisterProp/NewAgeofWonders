// TAOWComboBox.CheckMouseDown (/AoWInterface.dpl 0x0040f824)


void AoWComboBox_TAOWComboBox_CheckMouseDown_23EDC2EF
               (int *param_1,char param_2,undefined1 param_3,int param_4,int param_5)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
                    /* 0xf824  227  AoWComboBox.TAOWComboBox.CheckMouseDown@23EDC2EF */
  AoWControl_TAOWControl_CheckMouseDown_23EDC2EF(param_1,param_2,param_3,param_4,param_5);
  param_1[0x1b] = 0;
  if ((((param_1[0x45] != 0) &&
       (iVar5 = *(int *)(param_1[0x42] + 0x80) + param_1[0x20], iVar5 + -1 < param_5)) &&
      (param_5 < iVar5 + param_1[0x1e])) &&
     ((iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1[0x52],0),
      *(int *)(iVar5 + 0x24) + *(int *)(param_1[0x42] + 0x84) + param_1[0x21] + -1 < param_4 &&
      (iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1[0x52],0),
      param_4 < *(int *)(iVar5 + 0x24) + *(int *)(param_1[0x42] + 0x84) + param_1[0x21] +
                param_1[0x56] * param_1[0x48])))) {
    iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1[0x52],0);
    iVar5 = *(int *)(iVar5 + 0x20);
    iVar4 = *(int *)(param_1[0x42] + 0x80);
    iVar1 = param_1[0x20];
    iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1[0x52],1);
    if (param_5 < (int)(iVar5 + iVar4 + iVar1 +
                        *(int *)(iVar3 + 0x20) * (uint)*(byte *)(param_1 + 0x27) + param_1[0x28])) {
      param_1[0x1b] = -1;
    }
  }
  iVar5 = param_1[0x42];
  if (((*(int *)(iVar5 + 0x80) + param_1[0x20] + -1 < param_5) &&
      (param_5 < *(int *)(param_1[0x42] + 0x80) + param_1[0x20] + param_1[0x1e])) &&
     (*(int *)(iVar5 + 0x84) + param_1[0x21] + -1 < param_4)) {
    iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1[0x52],0);
    iVar5 = *(int *)(iVar4 + 0x24) + *(int *)(iVar5 + 0x84) + param_1[0x21];
    if (param_4 < iVar5) {
      param_1[0x1b] = -1;
      if (param_2 == '\0') {
        *(undefined1 *)(param_1 + 0x1a) = 1;
        param_1[0x44] = -1;
        if (((char)param_1[0xb] == '\x01') && (*(char *)((int)param_1 + 0x69) == '\x01')) {
          if (param_1[0x45] == 0) {
            if (param_1[0x4c] != param_1[0x2a]) {
              (**(code **)(*param_1 + 0x40))(param_1,param_1[0x4c]);
            }
            param_1[0x45] = -1;
            iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1[0x52],0);
            iVar5 = *(int *)(iVar5 + 0x24);
            iVar4 = param_1[0x56];
            iVar1 = param_1[0x48];
            iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1[0x53],5);
            param_1[0x1f] = iVar5 + iVar4 * iVar1 + *(int *)(iVar3 + 0x24);
            iVar5 = (**(code **)(*(int *)param_1[0x46] + 0x14))();
            if (iVar5 < param_1[0x48]) {
              iVar5 = (**(code **)(*(int *)param_1[0x46] + 0x14))();
              param_1[0x48] = iVar5;
            }
            iVar5 = (**(code **)(*(int *)param_1[0x46] + 0x14))();
            if (iVar5 < param_1[0x49] + param_1[0x48]) {
              iVar5 = (**(code **)(*(int *)param_1[0x46] + 0x14))();
              param_1[0x49] = iVar5 - param_1[0x48];
            }
            *(undefined1 *)((int)param_1 + 0xd2) = 0;
            iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1[0x53],4);
            param_1[0x50] = *(int *)(iVar5 + 0x20);
            *(undefined1 *)(param_1 + 0x51) = 0;
            iVar5 = param_1[0x47];
            if ((iVar5 != 0) && (0 < param_1[0x48])) {
              if (*(int *)(iVar5 + 0x108) != param_1[0x42]) {
                AoWControl_TAOWControl_SetAOWWindow_23EDC2EF();
              }
              if (param_1[0x4c] + 1 != ((int *)param_1[0x47])[0x2a]) {
                (**(code **)(*(int *)param_1[0x47] + 0x40))();
              }
              AoWVScroll_TAOWVScrollBar_SetFMin_23EDC2EF(param_1[0x47],0);
              iVar5 = (**(code **)(*(int *)param_1[0x46] + 0x14))();
              AoWVScroll_TAOWVScrollBar_SetFMax_23EDC2EF(param_1[0x47],iVar5 + -1);
              AoWVScroll_TAOWVScrollBar_SetFPos_23EDC2EF(param_1[0x47],param_1[0x49]);
              AoWVScroll_TAOWVScrollBar_SetFNum_23EDC2EF(param_1[0x47],param_1[0x48]);
              iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1[0x53],6);
              (**(code **)(*(int *)param_1[0x47] + 0x3c))
                        ((int *)param_1[0x47],*(int *)(iVar5 + 0x24) + param_1[0x56] * param_1[0x48]
                        );
              (**(code **)(*(int *)param_1[0x47] + 0x44))((int *)param_1[0x47],1);
              if ((*(char *)(param_1[0x47] + 0x144) == '\x01') &&
                 (iVar5 = (**(code **)(*(int *)param_1[0x46] + 0x14))(), iVar5 + -1 < param_1[0x48])
                 ) {
                (**(code **)(*(int *)param_1[0x47] + 0x44))((int *)param_1[0x47],0);
              }
              else {
                param_1[0x50] = *(int *)(param_1[0x47] + 0x78);
              }
              *(undefined1 *)(param_1 + 0x51) = *(undefined1 *)(param_1[0x47] + 0x69);
              (**(code **)(*(int *)param_1[0x47] + 0x50))();
              piVar2 = (int *)param_1[0x47];
              (**(code **)(*piVar2 + 0x30))(piVar2,(param_1[0x20] + param_1[0x1e]) - piVar2[0x1e]);
              iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1[0x52],0);
              (**(code **)(*(int *)param_1[0x47] + 0x34))
                        ((int *)param_1[0x47],*(int *)(iVar5 + 0x24) + param_1[0x21]);
              iVar5 = param_1[0x47];
              *(undefined1 *)(iVar5 + 0xd2) = 0;
            }
          }
          else {
            if (param_1[0x4d] != param_1[0x2a]) {
              param_1[0x2a] = param_1[0x4d];
            }
            param_1[0x45] = 0;
            iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1[0x52],0);
            param_1[0x1f] = *(int *)(iVar5 + 0x24);
            if (param_1[0x47] != 0) {
              (**(code **)(*(int *)param_1[0x47] + 0x44))((int *)param_1[0x47],0);
            }
            iVar5 = param_1[0x42];
            *(undefined1 *)(iVar5 + 0xd2) = 0;
          }
        }
        else {
          iVar5 = 0;
          if (param_1[0x45] != 0) {
            if (param_1[0x4d] != param_1[0x2a]) {
              (**(code **)(*param_1 + 0x40))(param_1,param_1[0x4d]);
            }
            param_1[0x45] = 0;
            iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1[0x52],0);
            param_1[0x1f] = *(int *)(iVar5 + 0x24);
            if (param_1[0x47] != 0) {
              (**(code **)(*(int *)param_1[0x47] + 0x44))((int *)param_1[0x47],0);
            }
            iVar5 = param_1[0x42];
            *(undefined1 *)(iVar5 + 0xd2) = 0;
          }
        }
      }
      if (*(short *)((int)param_1 + 0x3a) == 0) {
        return;
      }
      (*(code *)param_1[0xe])
                (param_1[0xf],param_1,param_2,param_4,param_5,
                 CONCAT31((int3)((uint)iVar5 >> 8),param_3));
      return;
    }
  }
  iVar5 = param_1[0x42];
  iVar4 = *(int *)(iVar5 + 0x80) + param_1[0x20];
  if ((((iVar4 + -1 < param_5) && (param_5 < iVar4 + param_1[0x1e])) &&
      (*(int *)(iVar5 + 0x84) + param_1[0x21] + -1 < param_4)) &&
     (param_4 < *(int *)(iVar5 + 0x84) + param_1[0x21] + param_1[0x1f])) {
    param_1[0x1b] = -1;
  }
  else if (param_1[0x45] != 0) {
    if (param_1[0x4d] != param_1[0x2a]) {
      (**(code **)(*param_1 + 0x40))(param_1,param_1[0x4d]);
    }
    param_1[0x45] = 0;
    iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1[0x52],0);
    param_1[0x1f] = *(int *)(iVar5 + 0x24);
    piVar2 = (int *)param_1[0x47];
    if (piVar2 != (int *)0x0) {
      (**(code **)(*piVar2 + 0x44))(piVar2,0);
    }
    *(undefined1 *)(param_1[0x42] + 0xd2) = 0;
  }
  return;
}

