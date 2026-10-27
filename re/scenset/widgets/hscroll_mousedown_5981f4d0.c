// TAOWHScrollBar.CheckMouseDown (/aowInt.dpl 0x5981f4d0)


void AoWHScroll_TAOWHScrollBar_CheckMouseDown_23EDC2EF
               (int param_1,uint param_2,uint param_3,int param_4,int param_5)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int local_c;
  
                    /* 0x1f4d0  514  AoWHScroll.TAOWHScrollBar.CheckMouseDown@23EDC2EF */
  AoWControl_TAOWControl_CheckMouseDown_23EDC2EF(param_1,param_2,param_3,param_4,param_5);
  *(undefined4 *)(param_1 + 0x78) = 0;
  if ((((*(char *)(param_1 + 0x75) == '\x01') &&
       (*(int *)(*(int *)(param_1 + 0x10c) + 0x84) + *(int *)(param_1 + 0x84) + -1 < param_5)) &&
      (param_5 < *(int *)(*(int *)(param_1 + 0x10c) + 0x84) + *(int *)(param_1 + 0x84) +
                 *(int *)(param_1 + 0x7c))) &&
     ((*(int *)(*(int *)(param_1 + 0x10c) + 0x88) + *(int *)(param_1 + 0x88) + -1 < param_4 &&
      (param_4 < *(int *)(*(int *)(param_1 + 0x10c) + 0x88) + *(int *)(param_1 + 0x88) +
                 *(int *)(param_1 + 0x80))))) {
    iVar1 = *(int *)(param_1 + 0x148);
    *(undefined4 *)(param_1 + 0x78) = 0xffffffff;
    if ((char)param_2 == '\0') {
      *(undefined1 *)(param_1 + 0x150) = 1;
      *(undefined1 *)(param_1 + 0x151) = 5;
      iVar2 = *(int *)(param_1 + 0x130);
      local_c = (iVar2 * *(int *)(param_1 + 0x148)) / *(int *)(param_1 + 0x14c);
      if (iVar2 < local_c + *(int *)(param_1 + 0x134)) {
        local_c = iVar2 - *(int *)(param_1 + 0x134);
      }
      if (*(char *)(param_1 + 0x160) != '\x01') {
        iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                          (*(undefined4 *)(param_1 + 0x11c),*(undefined4 *)(param_1 + 0x16c));
        if (param_5 < *(int *)(iVar2 + 0x20) +
                      *(int *)(*(int *)(param_1 + 0x10c) + 0x84) + *(int *)(param_1 + 0x84) + -1) {
          *(undefined1 *)(param_1 + 0x151) = 1;
          *(int *)(param_1 + 0x148) = *(int *)(param_1 + 0x148) - *(int *)(param_1 + 0x140);
          if (*(int *)(param_1 + 0x148) < *(int *)(param_1 + 0x138)) {
            *(int *)(param_1 + 0x148) = *(int *)(param_1 + 0x138);
          }
        }
        iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                          (*(undefined4 *)(param_1 + 0x118),*(undefined4 *)(param_1 + 0x164));
        if ((*(int *)(*(int *)(param_1 + 0x10c) + 0x84) + *(int *)(param_1 + 0x84) +
            *(int *)(param_1 + 0x7c)) - *(int *)(iVar2 + 0x20) < param_5) {
          *(undefined1 *)(param_1 + 0x151) = 4;
          *(int *)(param_1 + 0x148) = *(int *)(param_1 + 0x148) + *(int *)(param_1 + 0x140);
          iVar2 = (*(int *)(param_1 + 0x13c) + 1) - *(int *)(param_1 + 0x158);
          if (iVar2 < *(int *)(param_1 + 0x148)) {
            *(int *)(param_1 + 0x148) = iVar2;
          }
        }
      }
      if (*(char *)(param_1 + 0x151) == '\x05') {
        if (*(char *)(param_1 + 0x160) == '\0') {
          iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                            (*(undefined4 *)(param_1 + 0x11c),*(undefined4 *)(param_1 + 0x16c));
          if ((*(int *)(iVar2 + 0x20) +
               *(int *)(*(int *)(param_1 + 0x10c) + 0x84) + *(int *)(param_1 + 0x84) < param_5) &&
             (iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                (*(undefined4 *)(param_1 + 0x118),*(undefined4 *)(param_1 + 0x164)),
             param_5 < *(int *)(iVar2 + 0x20) +
                       *(int *)(*(int *)(param_1 + 0x10c) + 0x84) + *(int *)(param_1 + 0x84) +
                       local_c)) {
            iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                              (*(undefined4 *)(param_1 + 0x11c),*(undefined4 *)(param_1 + 0x16c));
            *(int *)(param_1 + 0x154) =
                 ((((param_5 - *(int *)(*(int *)(param_1 + 0x10c) + 0x84)) -
                   *(int *)(param_1 + 0x84)) - *(int *)(iVar2 + 0x20)) * *(int *)(param_1 + 0x14c))
                 / *(int *)(param_1 + 0x130) + *(int *)(param_1 + 0x138);
            *(undefined1 *)(param_1 + 0x151) = 2;
            *(int *)(param_1 + 0x148) = *(int *)(param_1 + 0x148) - *(int *)(param_1 + 0x144);
            if (*(int *)(param_1 + 0x148) < *(int *)(param_1 + 0x138)) {
              *(int *)(param_1 + 0x148) = *(int *)(param_1 + 0x138);
            }
          }
          iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                            (*(undefined4 *)(param_1 + 0x11c),*(undefined4 *)(param_1 + 0x16c));
          if ((*(int *)(iVar2 + 0x20) +
               *(int *)(*(int *)(param_1 + 0x10c) + 0x84) + *(int *)(param_1 + 0x84) + local_c +
               *(int *)(param_1 + 0x134) < param_5) &&
             (iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                (*(undefined4 *)(param_1 + 0x11c),*(undefined4 *)(param_1 + 0x16c)),
             param_5 < (*(int *)(*(int *)(param_1 + 0x10c) + 0x84) + *(int *)(param_1 + 0x84) +
                       *(int *)(param_1 + 0x7c)) - *(int *)(iVar2 + 0x20))) {
            iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                              (*(undefined4 *)(param_1 + 0x11c),*(undefined4 *)(param_1 + 0x16c));
            *(int *)(param_1 + 0x154) =
                 ((((param_5 - *(int *)(*(int *)(param_1 + 0x10c) + 0x84)) -
                   *(int *)(param_1 + 0x84)) - *(int *)(iVar2 + 0x20)) * *(int *)(param_1 + 0x14c))
                 / *(int *)(param_1 + 0x130) + *(int *)(param_1 + 0x138);
            *(undefined1 *)(param_1 + 0x151) = 3;
            *(int *)(param_1 + 0x148) = *(int *)(param_1 + 0x148) + *(int *)(param_1 + 0x144);
            iVar2 = (*(int *)(param_1 + 0x13c) + 1) - *(int *)(param_1 + 0x158);
            if (iVar2 < *(int *)(param_1 + 0x148)) {
              *(int *)(param_1 + 0x148) = iVar2;
            }
          }
          if (*(char *)(param_1 + 0x151) == '\x05') {
            *(int *)(param_1 + 0x154) = param_5 - local_c;
          }
        }
        else if (*(char *)(param_1 + 0x160) == '\x01') {
          iVar2 = *(int *)(*(int *)(param_1 + 0x10c) + 0x84);
          iVar3 = iVar2 + *(int *)(param_1 + 0x84);
          if ((iVar3 < param_5) && (param_5 < iVar3 + local_c)) {
            *(int *)(param_1 + 0x154) =
                 (((param_5 - iVar2) - *(int *)(param_1 + 0x84)) * *(int *)(param_1 + 0x14c)) /
                 *(int *)(param_1 + 0x130) + *(int *)(param_1 + 0x138);
            *(undefined1 *)(param_1 + 0x151) = 2;
            *(int *)(param_1 + 0x148) = *(int *)(param_1 + 0x148) - *(int *)(param_1 + 0x144);
            if (*(int *)(param_1 + 0x148) < *(int *)(param_1 + 0x138)) {
              *(int *)(param_1 + 0x148) = *(int *)(param_1 + 0x138);
            }
          }
          iVar2 = *(int *)(*(int *)(param_1 + 0x10c) + 0x84);
          iVar3 = iVar2 + *(int *)(param_1 + 0x84);
          if ((iVar3 + local_c + *(int *)(param_1 + 0x134) < param_5) &&
             (param_5 < iVar3 + *(int *)(param_1 + 0x7c))) {
            *(int *)(param_1 + 0x154) =
                 (((param_5 - iVar2) - *(int *)(param_1 + 0x84)) * *(int *)(param_1 + 0x14c)) /
                 *(int *)(param_1 + 0x130) + *(int *)(param_1 + 0x138);
            *(undefined1 *)(param_1 + 0x151) = 3;
            *(int *)(param_1 + 0x148) = *(int *)(param_1 + 0x148) + *(int *)(param_1 + 0x144);
            iVar2 = (*(int *)(param_1 + 0x13c) + 1) - *(int *)(param_1 + 0x158);
            if (iVar2 < *(int *)(param_1 + 0x148)) {
              *(int *)(param_1 + 0x148) = iVar2;
            }
          }
          if (*(char *)(param_1 + 0x151) == '\x05') {
            *(int *)(param_1 + 0x154) = param_5 - local_c;
          }
        }
      }
    }
    if ((iVar1 != *(int *)(param_1 + 0x148)) &&
       (*(undefined1 *)(param_1 + 0xa4) = 0, *(short *)(param_1 + 0x6e) != 0)) {
      (**(code **)(param_1 + 0x6c))(*(undefined4 *)(param_1 + 0x70),param_1);
    }
    if (*(short *)(param_1 + 0x46) != 0) {
      (**(code **)(param_1 + 0x44))
                (*(undefined4 *)(param_1 + 0x48),param_1,param_2 & 0xff,param_4,param_5,
                 param_3 & 0xff);
    }
  }
  return;
}

