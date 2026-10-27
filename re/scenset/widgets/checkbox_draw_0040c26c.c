// TAOWCheckBox.Draw (/AoWInterface.dpl 0x0040c26c)


void AoWCheckBox_TAOWCheckBox_Draw_23EDC2EF(int param_1,undefined4 *param_2)

{
  int iVar1;
  undefined4 uVar2;
  undefined1 *puVar3;
  int *piVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  uint uVar9;
  undefined4 *puVar10;
  int *piVar11;
  undefined4 *puVar12;
  byte bVar13;
  undefined1 **ppuVar14;
  undefined4 uVar15;
  undefined1 *puStack_68;
  undefined1 *puStack_64;
  undefined1 *local_60;
  undefined4 local_5c [4];
  int local_4c [9];
  undefined4 auStack_28 [2];
  int local_20 [4];
  
                    /* 0xc26c  192  AoWCheckBox.TAOWCheckBox.Draw@23EDC2EF */
  bVar13 = 0;
  local_60 = (undefined1 *)local_5c;
  puVar10 = local_5c;
  for (iVar8 = 0xf; iVar8 != 0; iVar8 = iVar8 + -1) {
    *puVar10 = *param_2;
    param_2 = param_2 + 1;
    puVar10 = puVar10 + 1;
  }
  if ((*(char *)(param_1 + 0x2c) == '\x01') && (*(char *)(param_1 + 0x69) == '\x01')) {
    if (*(char *)(param_1 + 0xe0) == '\0') {
      if (*(char *)(param_1 + 0x110) == '\0') {
        piVar4 = local_4c;
        piVar11 = local_20;
        for (iVar8 = 4; iVar8 != 0; iVar8 = iVar8 + -1) {
          *piVar11 = *piVar4;
          piVar4 = piVar4 + 1;
          piVar11 = piVar11 + 1;
        }
        puStack_64 = (undefined1 *)0x40c5eb;
        local_60 = (undefined1 *)local_5c;
        piVar4 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(param_1 + 0x114),0);
        puStack_64 = (undefined1 *)0x40c611;
        (**(code **)(*piVar4 + 0x80))
                  (piVar4,*(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80),
                   *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84));
        uVar9 = (uint)*(byte *)(param_1 + 0x9c);
        if (-1 < (int)(uVar9 - 1)) {
          iVar8 = 0;
          do {
            puStack_64 = (undefined1 *)&local_60;
            puStack_68 = (undefined1 *)0x40c62f;
            iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),0);
            iVar5 = *(int *)(iVar5 + 0x20);
            iVar1 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
            iVar7 = *(int *)(param_1 + 0x80);
            puStack_68 = (undefined1 *)0x40c656;
            iVar6 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),1);
            puStack_68 = (undefined1 *)(iVar5 + iVar1 + iVar7 + *(int *)(iVar6 + 0x20) * iVar8);
            piVar4 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                      (*(undefined4 *)(param_1 + 0x114),1);
            puVar3 = puStack_68;
            puStack_68 = (undefined1 *)0x40c689;
            (**(code **)(*piVar4 + 0x80))
                      (piVar4,puVar3,
                       *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84));
            iVar8 = iVar8 + 1;
            uVar9 = uVar9 - 1;
          } while (uVar9 != 0);
        }
        puStack_64 = (undefined1 *)0x40c69a;
        iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),0);
        iVar8 = *(int *)(iVar8 + 0x20);
        iVar5 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
        iVar1 = *(int *)(param_1 + 0x80);
        puStack_64 = (undefined1 *)0x40c6c1;
        iVar7 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),1);
        if ((int)(iVar8 + iVar5 + iVar1 + *(int *)(iVar7 + 0x20) * (uint)*(byte *)(param_1 + 0x9c) +
                 *(int *)(param_1 + 0xa0)) < local_20[1]) {
          puStack_64 = (undefined1 *)0x40c6e9;
          iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),0);
          iVar8 = *(int *)(iVar8 + 0x20);
          iVar5 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
          iVar1 = *(int *)(param_1 + 0x80);
          puStack_64 = (undefined1 *)0x40c710;
          iVar7 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),1);
          local_4c[1] = iVar8 + iVar5 + iVar1 +
                        *(int *)(iVar7 + 0x20) * (uint)*(byte *)(param_1 + 0x9c) +
                        *(int *)(param_1 + 0xa0);
        }
        puStack_64 = (undefined1 *)&local_60;
        puStack_68 = (undefined1 *)0x40c737;
        iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),0);
        iVar8 = *(int *)(iVar8 + 0x20);
        iVar5 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
        iVar1 = *(int *)(param_1 + 0x80);
        puStack_68 = (undefined1 *)0x40c75e;
        iVar7 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),1);
        puStack_68 = (undefined1 *)
                     (iVar8 + iVar5 + iVar1 +
                     *(int *)(iVar7 + 0x20) * (uint)*(byte *)(param_1 + 0x9c));
        piVar4 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(param_1 + 0x114),1);
        puVar3 = puStack_68;
        puStack_68 = (undefined1 *)0x40c799;
        (**(code **)(*piVar4 + 0x80))
                  (piVar4,puVar3,
                   *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84));
        puVar10 = auStack_28;
        puVar12 = local_5c + 2;
        for (iVar8 = 4; iVar8 != 0; iVar8 = iVar8 + -1) {
          *puVar12 = *puVar10;
          puVar10 = puVar10 + (uint)bVar13 * -2 + 1;
          puVar12 = puVar12 + (uint)bVar13 * -2 + 1;
        }
        puStack_68 = (undefined1 *)&puStack_64;
        iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),0);
        iVar8 = *(int *)(iVar8 + 0x20);
        iVar5 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
        iVar1 = *(int *)(param_1 + 0x80);
        iVar7 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),1);
        iVar8 = iVar8 + iVar5 + iVar1 + *(int *)(iVar7 + 0x20) * (uint)*(byte *)(param_1 + 0x9c) +
                *(int *)(param_1 + 0xa0);
        piVar4 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(param_1 + 0x114),2);
        (**(code **)(*piVar4 + 0x80))
                  (piVar4,iVar8,
                   *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84));
      }
      else {
        puStack_64 = (undefined1 *)0x40c830;
        piVar4 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(param_1 + 0x118),0);
        puStack_64 = (undefined1 *)0x40c856;
        (**(code **)(*piVar4 + 0x80))
                  (piVar4,*(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80),
                   *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84));
      }
      if (*(int *)(param_1 + 0x88) != 0) {
        ppuVar14 = &puStack_68;
        ImageLib_TImageLibraryFont_SetColor_23EDC2EF
                  (*(int *)(param_1 + 0x88),*(undefined4 *)(param_1 + 0x24));
        uVar15 = *(undefined4 *)(param_1 + 0x8c);
        uVar2 = *(undefined4 *)(param_1 + 0x88);
        uVar9 = ImageLib_TImageLibraryFont_GetTextWidth_23EDC2EF
                          (uVar2,*(undefined4 *)(param_1 + 0x8c));
        iVar8 = (int)uVar9 >> 1;
        if (iVar8 < 0) {
          iVar8 = iVar8 + (uint)((uVar9 & 1) != 0);
        }
        iVar5 = (int)*(uint *)(param_1 + 0x78) >> 1;
        if (iVar5 < 0) {
          iVar5 = iVar5 + (uint)((*(uint *)(param_1 + 0x78) & 1) != 0);
        }
        ImageLib_TImageLibraryFont_SetText_23EDC2EF
                  (uVar2,(*(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80) +
                         iVar5) - iVar8,
                   *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84) +
                   *(int *)(param_1 + 0x98),ppuVar14,uVar15);
      }
    }
    else {
      if (*(char *)(param_1 + 0x110) == '\0') {
        piVar4 = local_4c;
        piVar11 = local_20;
        for (iVar8 = 4; iVar8 != 0; iVar8 = iVar8 + -1) {
          *piVar11 = *piVar4;
          piVar4 = piVar4 + 1;
          piVar11 = piVar11 + 1;
        }
        puStack_64 = (undefined1 *)0x40c2d1;
        local_60 = (undefined1 *)local_5c;
        piVar4 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(param_1 + 0x114),3);
        puStack_64 = (undefined1 *)0x40c2f7;
        (**(code **)(*piVar4 + 0x80))
                  (piVar4,*(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80),
                   *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84));
        uVar9 = (uint)*(byte *)(param_1 + 0x9c);
        if (-1 < (int)(uVar9 - 1)) {
          iVar8 = 0;
          do {
            puStack_64 = (undefined1 *)&local_60;
            puStack_68 = (undefined1 *)0x40c315;
            iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),0);
            iVar5 = *(int *)(iVar5 + 0x20);
            iVar1 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
            iVar7 = *(int *)(param_1 + 0x80);
            puStack_68 = (undefined1 *)0x40c33c;
            iVar6 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),1);
            puStack_68 = (undefined1 *)(iVar5 + iVar1 + iVar7 + *(int *)(iVar6 + 0x20) * iVar8);
            piVar4 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                      (*(undefined4 *)(param_1 + 0x114),4);
            puVar3 = puStack_68;
            puStack_68 = (undefined1 *)0x40c36f;
            (**(code **)(*piVar4 + 0x80))
                      (piVar4,puVar3,
                       *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84));
            iVar8 = iVar8 + 1;
            uVar9 = uVar9 - 1;
          } while (uVar9 != 0);
        }
        puStack_64 = (undefined1 *)0x40c380;
        iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),0);
        iVar8 = *(int *)(iVar8 + 0x20);
        iVar5 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
        iVar1 = *(int *)(param_1 + 0x80);
        puStack_64 = (undefined1 *)0x40c3a7;
        iVar7 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),1);
        if ((int)(iVar8 + iVar5 + iVar1 + *(int *)(iVar7 + 0x20) * (uint)*(byte *)(param_1 + 0x9c) +
                 *(int *)(param_1 + 0xa0)) < local_20[1]) {
          puStack_64 = (undefined1 *)0x40c3cf;
          iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),0);
          iVar8 = *(int *)(iVar8 + 0x20);
          iVar5 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
          iVar1 = *(int *)(param_1 + 0x80);
          puStack_64 = (undefined1 *)0x40c3f6;
          iVar7 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),1);
          local_4c[1] = iVar8 + iVar5 + iVar1 +
                        *(int *)(iVar7 + 0x20) * (uint)*(byte *)(param_1 + 0x9c) +
                        *(int *)(param_1 + 0xa0);
        }
        puStack_64 = (undefined1 *)&local_60;
        puStack_68 = (undefined1 *)0x40c41d;
        iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),0);
        iVar8 = *(int *)(iVar8 + 0x20);
        iVar5 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
        iVar1 = *(int *)(param_1 + 0x80);
        puStack_68 = (undefined1 *)0x40c444;
        iVar7 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),1);
        puStack_68 = (undefined1 *)
                     (iVar8 + iVar5 + iVar1 +
                     *(int *)(iVar7 + 0x20) * (uint)*(byte *)(param_1 + 0x9c));
        piVar4 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(param_1 + 0x114),4);
        puVar3 = puStack_68;
        puStack_68 = (undefined1 *)0x40c47f;
        (**(code **)(*piVar4 + 0x80))
                  (piVar4,puVar3,
                   *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84));
        puVar10 = auStack_28;
        puVar12 = local_5c + 2;
        for (iVar8 = 4; iVar8 != 0; iVar8 = iVar8 + -1) {
          *puVar12 = *puVar10;
          puVar10 = puVar10 + (uint)bVar13 * -2 + 1;
          puVar12 = puVar12 + (uint)bVar13 * -2 + 1;
        }
        puStack_68 = (undefined1 *)&puStack_64;
        iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),0);
        iVar8 = *(int *)(iVar8 + 0x20);
        iVar5 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
        iVar1 = *(int *)(param_1 + 0x80);
        iVar7 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),1);
        iVar8 = iVar8 + iVar5 + iVar1 + *(int *)(iVar7 + 0x20) * (uint)*(byte *)(param_1 + 0x9c) +
                *(int *)(param_1 + 0xa0);
        piVar4 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(param_1 + 0x114),5);
        (**(code **)(*piVar4 + 0x80))
                  (piVar4,iVar8,
                   *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84));
      }
      else {
        puStack_64 = (undefined1 *)0x40c519;
        local_60 = (undefined1 *)local_5c;
        piVar4 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(param_1 + 0x118),1);
        puStack_64 = (undefined1 *)0x40c53f;
        (**(code **)(*piVar4 + 0x80))
                  (piVar4,*(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80),
                   *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84));
      }
      if (*(int *)(param_1 + 0x88) != 0) {
        ppuVar14 = &puStack_68;
        ImageLib_TImageLibraryFont_SetColor_23EDC2EF
                  (*(int *)(param_1 + 0x88),*(undefined4 *)(param_1 + 0x28));
        uVar15 = *(undefined4 *)(param_1 + 0x8c);
        uVar2 = *(undefined4 *)(param_1 + 0x88);
        uVar9 = ImageLib_TImageLibraryFont_GetTextWidth_23EDC2EF
                          (uVar2,*(undefined4 *)(param_1 + 0x8c));
        iVar8 = (int)uVar9 >> 1;
        if (iVar8 < 0) {
          iVar8 = iVar8 + (uint)((uVar9 & 1) != 0);
        }
        iVar5 = (int)*(uint *)(param_1 + 0x78) >> 1;
        if (iVar5 < 0) {
          iVar5 = iVar5 + (uint)((*(uint *)(param_1 + 0x78) & 1) != 0);
        }
        ImageLib_TImageLibraryFont_SetText_23EDC2EF
                  (uVar2,(*(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80) +
                         iVar5) - iVar8,
                   *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84) +
                   *(int *)(param_1 + 0x11c),ppuVar14,uVar15);
      }
    }
    if (*(short *)(param_1 + 0x32) != 0) {
      (**(code **)(param_1 + 0x30))(*(undefined4 *)(param_1 + 0x34),param_1,&puStack_68);
    }
  }
  return;
}

