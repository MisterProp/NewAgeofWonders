// TAOWHScrollBar.SetSize (/aowInt.dpl 0x5981e8f0)


void AoWHScroll_TAOWHScrollBar_SetSize_23EDC2EF(int param_1)

{
  char cVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
                    /* 0x1e8f0  516  AoWHScroll.TAOWHScrollBar.SetSize@23EDC2EF */
  iVar2 = (*(int *)(param_1 + 0x13c) - *(int *)(param_1 + 0x138)) + 1;
  *(int *)(param_1 + 0x14c) = iVar2;
  if (iVar2 < *(int *)(param_1 + 0x158)) {
    *(int *)(param_1 + 0x158) = iVar2;
  }
  cVar1 = *(char *)(param_1 + 0x160);
  if (cVar1 == '\0') {
    iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                      (*(undefined4 *)(param_1 + 0x120),*(undefined4 *)(param_1 + 0x174));
    *(undefined4 *)(param_1 + 0x80) = *(undefined4 *)(iVar2 + 0x24);
    iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                      (*(undefined4 *)(param_1 + 0x11c),*(undefined4 *)(param_1 + 0x16c));
    iVar2 = *(int *)(iVar2 + 0x20);
    iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                      (*(undefined4 *)(param_1 + 0x118),*(undefined4 *)(param_1 + 0x164));
    if (*(int *)(param_1 + 0x7c) < iVar2 + *(int *)(iVar4 + 0x20)) {
      iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                        (*(undefined4 *)(param_1 + 0x11c),*(undefined4 *)(param_1 + 0x16c));
      iVar2 = *(int *)(iVar2 + 0x20);
      iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                        (*(undefined4 *)(param_1 + 0x118),*(undefined4 *)(param_1 + 0x164));
      *(int *)(param_1 + 0x7c) = iVar2 + *(int *)(iVar4 + 0x20);
    }
    iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                      (*(undefined4 *)(param_1 + 0x11c),*(undefined4 *)(param_1 + 0x16c));
    iVar2 = *(int *)(param_1 + 0x7c);
    iVar4 = *(int *)(iVar4 + 0x20);
    iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                      (*(undefined4 *)(param_1 + 0x118),*(undefined4 *)(param_1 + 0x164));
    iVar2 = (iVar2 - iVar4) - *(int *)(iVar3 + 0x20);
    *(int *)(param_1 + 0x130) = iVar2;
    iVar2 = (iVar2 * *(int *)(param_1 + 0x158)) / *(int *)(param_1 + 0x14c);
    *(int *)(param_1 + 0x134) = iVar2;
    if (*(char *)(param_1 + 0x161) == '\0') {
      iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                        (*(undefined4 *)(param_1 + 0x128),*(undefined4 *)(param_1 + 0x178));
      *(undefined4 *)(param_1 + 0x134) = *(undefined4 *)(iVar2 + 0x20);
    }
    else if (*(char *)(param_1 + 0x161) == '\x01') {
      iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                        (*(undefined4 *)(param_1 + 300),*(undefined4 *)(param_1 + 0x178));
      iVar4 = *(int *)(iVar4 + 0x20);
      iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                        (*(undefined4 *)(param_1 + 300),*(int *)(param_1 + 0x178) + 2);
      if (iVar2 < iVar4 + *(int *)(iVar3 + 0x20)) {
        iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                          (*(undefined4 *)(param_1 + 300),*(undefined4 *)(param_1 + 0x178));
        iVar2 = *(int *)(iVar2 + 0x20);
        iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                          (*(undefined4 *)(param_1 + 300),*(int *)(param_1 + 0x178) + 2);
        *(int *)(param_1 + 0x134) = iVar2 + *(int *)(iVar4 + 0x20);
      }
    }
  }
  else if (cVar1 == '\x01') {
    iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                      (*(undefined4 *)(param_1 + 0x124),*(undefined4 *)(param_1 + 0x174));
    *(undefined4 *)(param_1 + 0x80) = *(undefined4 *)(iVar2 + 0x24);
    *(int *)(param_1 + 0x130) = *(int *)(param_1 + 0x7c);
    iVar2 = (*(int *)(param_1 + 0x7c) * *(int *)(param_1 + 0x158)) / *(int *)(param_1 + 0x14c);
    *(int *)(param_1 + 0x134) = iVar2;
    if (*(char *)(param_1 + 0x161) == '\0') {
      iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                        (*(undefined4 *)(param_1 + 0x128),*(undefined4 *)(param_1 + 0x178));
      *(undefined4 *)(param_1 + 0x134) = *(undefined4 *)(iVar2 + 0x20);
    }
    else if (*(char *)(param_1 + 0x161) == '\x01') {
      iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                        (*(undefined4 *)(param_1 + 300),*(undefined4 *)(param_1 + 0x178));
      iVar4 = *(int *)(iVar4 + 0x20);
      iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                        (*(undefined4 *)(param_1 + 300),*(int *)(param_1 + 0x178) + 2);
      if (iVar2 < iVar4 + *(int *)(iVar3 + 0x20)) {
        iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                          (*(undefined4 *)(param_1 + 300),*(undefined4 *)(param_1 + 0x178));
        iVar2 = *(int *)(iVar2 + 0x20);
        iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                          (*(undefined4 *)(param_1 + 300),*(int *)(param_1 + 0x178) + 2);
        *(int *)(param_1 + 0x134) = iVar2 + *(int *)(iVar4 + 0x20);
      }
    }
  }
  else if (cVar1 == '\x02') {
    iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                      (*(undefined4 *)(param_1 + 0x11c),*(undefined4 *)(param_1 + 0x16c));
    *(undefined4 *)(param_1 + 0x80) = *(undefined4 *)(iVar2 + 0x24);
    iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                      (*(undefined4 *)(param_1 + 0x11c),*(undefined4 *)(param_1 + 0x16c));
    iVar2 = *(int *)(iVar2 + 0x20);
    iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                      (*(undefined4 *)(param_1 + 0x118),*(undefined4 *)(param_1 + 0x164));
    *(int *)(param_1 + 0x7c) = iVar2 + *(int *)(iVar4 + 0x20);
  }
  if (*(int *)(param_1 + 0x14c) < *(int *)(param_1 + 0x148) + *(int *)(param_1 + 0x158)) {
    *(int *)(param_1 + 0x148) = *(int *)(param_1 + 0x14c) - *(int *)(param_1 + 0x158);
  }
  AoWComp_TAoWComponent_SetSize_23EDC2EF(param_1);
  return;
}

