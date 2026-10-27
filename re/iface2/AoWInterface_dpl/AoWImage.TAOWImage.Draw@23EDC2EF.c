// /AoWInterface.dpl : AoWImage.TAOWImage.Draw@23EDC2EF

void AoWImage_TAOWImage_Draw_23EDC2EF(int param_1,undefined4 *param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int *piVar7;
  int iVar8;
  undefined4 *puVar9;
  int iVar10;
  int *piVar11;
  byte bVar12;
  int iStack_64;
  int iStack_60;
  undefined4 local_5c [4];
  int local_4c [11];
  int local_20 [4];
  
                    /* 0x1194c  267  AoWImage.TAOWImage.Draw@23EDC2EF */
  bVar12 = 0;
  puVar9 = local_5c;
  for (iVar8 = 0xf; iVar8 != 0; iVar8 = iVar8 + -1) {
    *puVar9 = *param_2;
    param_2 = param_2 + 1;
    puVar9 = puVar9 + 1;
  }
  if ((*(char *)(param_1 + 0x2c) == '\x01') && (*(char *)(param_1 + 0x69) == '\x01')) {
    piVar7 = local_4c;
    piVar11 = local_20;
    for (iVar8 = 4; iVar8 != 0; iVar8 = iVar8 + -1) {
      *piVar11 = *piVar7;
      piVar7 = piVar7 + 1;
      piVar11 = piVar11 + 1;
    }
    iVar8 = *(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80) +
            *(int *)(param_1 + 0x78);
    if (iVar8 < local_20[2]) {
      local_4c[2] = iVar8;
    }
    iVar8 = *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84) +
            *(int *)(param_1 + 0x7c);
    if (iVar8 < local_20[3]) {
      local_4c[3] = iVar8;
    }
    iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                      (*(undefined4 *)(param_1 + 0x70),*(undefined4 *)(param_1 + 0x11c));
    iStack_60 = *(int *)(param_1 + 0x7c) / *(int *)(iVar8 + 0x24);
    if (-1 < iStack_60) {
      iStack_60 = iStack_60 + 1;
      iStack_64 = 0;
      do {
        iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                          (*(undefined4 *)(param_1 + 0x70),*(undefined4 *)(param_1 + 0x11c));
        iVar8 = *(int *)(param_1 + 0x78) / *(int *)(iVar8 + 0x20);
        if (-1 < iVar8) {
          iVar8 = iVar8 + 1;
          iVar10 = 0;
          do {
            puVar9 = local_5c;
            iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                              (*(undefined4 *)(param_1 + 0x70),*(undefined4 *)(param_1 + 0x11c));
            iVar5 = *(int *)(iVar5 + 0x24);
            iVar1 = *(int *)(*(int *)(param_1 + 0x108) + 0x84);
            iVar2 = *(int *)(param_1 + 0x84);
            iVar6 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                              (*(undefined4 *)(param_1 + 0x70),*(undefined4 *)(param_1 + 0x11c));
            iVar6 = *(int *)(iVar6 + 0x20);
            iVar3 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
            iVar4 = *(int *)(param_1 + 0x80);
            piVar7 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                      (*(undefined4 *)(param_1 + 0x70),
                                       *(undefined4 *)(param_1 + 0x11c));
            (**(code **)(*piVar7 + 0x80))
                      (piVar7,iVar6 * iVar10 + iVar3 + iVar4,iVar5 * iStack_64 + iVar1 + iVar2,
                       puVar9);
            iVar10 = iVar10 + 1;
            iVar8 = iVar8 + -1;
          } while (iVar8 != 0);
        }
        iStack_64 = iStack_64 + 1;
        iStack_60 = iStack_60 + -1;
      } while (iStack_60 != 0);
    }
    piVar7 = local_20;
    piVar11 = local_4c;
    for (iVar8 = 4; iVar8 != 0; iVar8 = iVar8 + -1) {
      *piVar11 = *piVar7;
      piVar7 = piVar7 + (uint)bVar12 * -2 + 1;
      piVar11 = piVar11 + (uint)bVar12 * -2 + 1;
    }
    if (*(short *)(param_1 + 0x32) != 0) {
      (**(code **)(param_1 + 0x30))(*(undefined4 *)(param_1 + 0x34),param_1,local_5c);
    }
  }
  return;
}

