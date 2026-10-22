void AoWListBox_TAOWListBox_Draw_23EDC2EF(int param_1,undefined4 *param_2)
{
  byte bVar1;
  undefined1 *puVar2;
  int *piVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  undefined4 *puVar12;
  int iVar13;
  undefined4 *puVar14;
  undefined4 *in_FS_OFFSET;
  byte bVar15;
  undefined4 uVar16;
  undefined4 uStack_90;
  undefined1 *puStack_8c;
  undefined1 *puStack_88;
  undefined4 local_78 [4];
  undefined4 local_68;
  undefined4 local_64 [2];
  int local_5c;
  int local_58;
  undefined4 local_54 [4];
  undefined4 local_44 [2];
  int local_3c;
  int local_38;
  int local_18;
  int local_14;
  uint local_10;
  int local_c;
  int local_8;
                    /* 0x12c6c  295  AoWListBox.TAOWListBox.Draw@23EDC2EF */
  bVar15 = 0;
  puStack_88 = &stack0xfffffffc;
  local_68 = 0;
  puVar12 = local_54;
  for (iVar10 = 0xf; iVar10 != 0; iVar10 = iVar10 + -1) {
    *puVar12 = *param_2;
    param_2 = param_2 + 1;
    puVar12 = puVar12 + 1;
  }
  puStack_8c = &LAB_0041388f;
  uStack_90 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_90;
  puVar2 = &stack0xfffffffc;
  if ((*(char *)(param_1 + 0x2c) == '\x01') &&
     (puVar2 = &stack0xfffffffc, *(char *)(param_1 + 0x69) == '\x01')) {
    puVar2 = &stack0xfffffffc;
    if (*(int *)(param_1 + 0x88) != 0) {
      ImageLib_TImageLibraryFont_SetColor_23EDC2EF
                (*(int *)(param_1 + 0x88),*(undefined4 *)(param_1 + 0x24));
      puVar2 = puStack_88;
    }
    puStack_88 = puVar2;
    puVar12 = local_44;
    puVar14 = local_64;
    for (iVar10 = 4; iVar10 != 0; iVar10 = iVar10 + -1) {
      *puVar14 = *puVar12;
      puVar12 = puVar12 + (uint)bVar15 * -2 + 1;
      puVar14 = puVar14 + (uint)bVar15 * -2 + 1;
    }
    piVar3 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                              (*(undefined4 *)(param_1 + 0x134),0,0,local_54);
    (**(code **)(*piVar3 + 0x80))
              (piVar3,*(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80),
               *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84));
    iVar10 = (*(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80) +
             *(int *)(param_1 + 0x78)) - *(int *)(param_1 + 300);
    if (iVar10 < local_5c) {
      local_3c = iVar10;
    }
    if (-1 < (int)(*(byte *)(param_1 + 0x9c) - 1)) {
      local_8 = 0;
      local_10 = (uint)*(byte *)(param_1 + 0x9c);
      do {
        puVar12 = local_54;
        iVar10 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),0);
        iVar10 = *(int *)(iVar10 + 0x20);
        iVar5 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
        iVar6 = *(int *)(param_1 + 0x80);
        iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),1);
        iVar10 = iVar10 + iVar5 + iVar6 + *(int *)(iVar4 + 0x20) * local_8;
        piVar3 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(param_1 + 0x134),1);
        (**(code **)(*piVar3 + 0x80))
                  (piVar3,iVar10,
                   *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84),puVar12);
        local_8 = local_8 + 1;
        local_10 = local_10 - 1;
      } while (local_10 != 0);
    }
    puVar12 = local_54;
    iVar10 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),0);
    iVar10 = *(int *)(iVar10 + 0x20);
    iVar5 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
    iVar6 = *(int *)(param_1 + 0x80);
    iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),1);
    iVar10 = iVar10 + iVar5 + iVar6 + *(int *)(iVar4 + 0x20) * (uint)*(byte *)(param_1 + 0x9c);
    piVar3 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),1);
    (**(code **)(*piVar3 + 0x80))
              (piVar3,iVar10,*(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84),
               puVar12);
    puVar12 = local_64;
    puVar14 = local_44;
    for (iVar10 = 4; iVar10 != 0; iVar10 = iVar10 + -1) {
      *puVar14 = *puVar12;
      puVar12 = puVar12 + (uint)bVar15 * -2 + 1;
      puVar14 = puVar14 + (uint)bVar15 * -2 + 1;
    }
    iVar10 = *(int *)(param_1 + 0x108);
    iVar5 = (*(int *)(iVar10 + 0x80) + *(int *)(param_1 + 0x80) + *(int *)(param_1 + 0x78)) -
            *(int *)(param_1 + 300);
    if (iVar5 < local_5c) {
      local_3c = iVar5;
    }
    iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),6);
    if ((*(int *)(iVar10 + 0x84) + *(int *)(param_1 + 0x84) + *(int *)(param_1 + 0x7c)) -
        *(int *)(iVar5 + 0x24) < local_58) {
      iVar10 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),6);
      local_38 = (*(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84) +
                 *(int *)(param_1 + 0x7c)) - *(int *)(iVar10 + 0x24);
    }
    iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),1);
    iVar10 = *(int *)(param_1 + 0x7c);
    iVar5 = *(int *)(iVar5 + 0x24);
    iVar6 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),6);
    iVar6 = *(int *)(iVar6 + 0x24);
    iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x138),0);
    iVar10 = ((iVar10 - iVar5) - iVar6) / *(int *)(iVar4 + 0x24);
    if (-1 < iVar10) {
      local_10 = iVar10 + 1;
      local_c = 0;
      do {
        iVar10 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x138),0);
        iVar10 = (*(int *)(param_1 + 0x78) - *(int *)(param_1 + 300)) / *(int *)(iVar10 + 0x20);
        if (-1 < iVar10) {
          local_14 = iVar10 + 1;
          iVar10 = 0;
          do {
            iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),1);
            iVar5 = *(int *)(iVar5 + 0x24);
            iVar6 = *(int *)(*(int *)(param_1 + 0x108) + 0x84);
            iVar4 = *(int *)(param_1 + 0x84);
            iVar7 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x138),0);
            iVar13 = iVar5 + iVar6 + iVar4 + *(int *)(iVar7 + 0x24) * local_c;
            iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),3);
            iVar5 = *(int *)(iVar5 + 0x20);
            iVar6 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
            iVar4 = *(int *)(param_1 + 0x80);
            iVar7 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x138),0);
            iVar7 = *(int *)(iVar7 + 0x20);
            piVar3 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                      (*(undefined4 *)(param_1 + 0x138),0);
            (**(code **)(*piVar3 + 0x80))(piVar3,iVar5 + iVar6 + iVar4 + iVar7 * iVar10,iVar13);
            iVar10 = iVar10 + 1;
            local_14 = local_14 + -1;
          } while (local_14 != 0);
        }
        local_c = local_c + 1;
        local_10 = local_10 - 1;
      } while (local_10 != 0);
    }
    puVar12 = local_64;
    puVar14 = local_44;
    for (iVar10 = 4; iVar10 != 0; iVar10 = iVar10 + -1) {
      *puVar14 = *puVar12;
      puVar12 = puVar12 + (uint)bVar15 * -2 + 1;
      puVar14 = puVar14 + (uint)bVar15 * -2 + 1;
    }
    iVar10 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),5);
    if ((*(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84) +
        *(int *)(param_1 + 0x7c)) - *(int *)(iVar10 + 0x24) < local_58) {
      iVar10 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),5);
      local_38 = (*(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84) +
                 *(int *)(param_1 + 0x7c)) - *(int *)(iVar10 + 0x24);
    }
    iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),0);
    iVar10 = *(int *)(param_1 + 0x7c);
    iVar5 = *(int *)(iVar5 + 0x24);
    iVar6 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),5);
    iVar6 = *(int *)(iVar6 + 0x24);
    iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),3);
    iVar10 = ((iVar10 - iVar5) - iVar6) / *(int *)(iVar4 + 0x24);
    if (-1 < iVar10) {
      local_10 = iVar10 + 1;
      local_c = 0;
      do {
        puVar12 = local_54;
        iVar10 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),0);
        iVar10 = *(int *)(iVar10 + 0x24);
        iVar5 = *(int *)(param_1 + 0x108);
        iVar6 = *(int *)(iVar5 + 0x84);
        iVar4 = *(int *)(param_1 + 0x84);
        iVar7 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),3);
        iVar10 = iVar10 + iVar6 + iVar4 + *(int *)(iVar7 + 0x24) * local_c;
        piVar3 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(param_1 + 0x134),3);
        (**(code **)(*piVar3 + 0x80))
                  (piVar3,*(int *)(iVar5 + 0x80) + *(int *)(param_1 + 0x80),iVar10,puVar12);
        local_c = local_c + 1;
        local_10 = local_10 - 1;
      } while (local_10 != 0);
    }
    if (*(char *)(param_1 + 0x130) == '\0') {
      iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),2);
      iVar10 = *(int *)(param_1 + 0x7c);
      iVar5 = *(int *)(iVar5 + 0x24);
      iVar6 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),7);
      iVar6 = *(int *)(iVar6 + 0x24);
      iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),4);
      iVar10 = ((iVar10 - iVar5) - iVar6) / *(int *)(iVar4 + 0x24);
      if (-1 < iVar10) {
        local_10 = iVar10 + 1;
        local_c = 0;
        do {
          puVar12 = local_54;
          iVar10 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),0);
          iVar10 = *(int *)(iVar10 + 0x24);
          iVar5 = *(int *)(param_1 + 0x108);
          iVar6 = *(int *)(iVar5 + 0x84);
          iVar4 = *(int *)(param_1 + 0x84);
          iVar7 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),4);
          iVar10 = iVar10 + iVar6 + iVar4 + *(int *)(iVar7 + 0x24) * local_c;
          piVar3 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                    (*(undefined4 *)(param_1 + 0x134),4);
          (**(code **)(*piVar3 + 0x80))
                    (piVar3,(*(int *)(iVar5 + 0x80) + *(int *)(param_1 + 0x80) +
                            *(int *)(param_1 + 0x78)) - *(int *)(param_1 + 300),iVar10,puVar12);
          local_c = local_c + 1;
          local_10 = local_10 - 1;
        } while (local_10 != 0);
      }
    }
    puVar12 = local_64;
    puVar14 = local_44;
    for (iVar10 = 4; iVar10 != 0; iVar10 = iVar10 + -1) {
      *puVar14 = *puVar12;
      puVar12 = puVar12 + (uint)bVar15 * -2 + 1;
      puVar14 = puVar14 + (uint)bVar15 * -2 + 1;
    }
    if (*(char *)(param_1 + 0x130) == '\0') {
      piVar3 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                (*(undefined4 *)(param_1 + 0x134),2,0,local_54);
      (**(code **)(*piVar3 + 0x80))
                (piVar3,(*(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80) +
                        *(int *)(param_1 + 0x78)) - *(int *)(param_1 + 300),
                 *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84));
    }
    if (*(char *)(param_1 + 0x130) == '\0') {
      puVar12 = local_54;
      iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),7);
      iVar10 = *(int *)(param_1 + 0x108);
      iVar5 = (*(int *)(iVar10 + 0x84) + *(int *)(param_1 + 0x84) + *(int *)(param_1 + 0x7c)) -
              *(int *)(iVar5 + 0x24);
      piVar3 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),7);
      (**(code **)(*piVar3 + 0x80))
                (piVar3,(*(int *)(iVar10 + 0x80) + *(int *)(param_1 + 0x80) +
                        *(int *)(param_1 + 0x78)) - *(int *)(param_1 + 300),iVar5,puVar12);
    }
    if (-1 < (int)(*(uint *)(param_1 + 0x118) - 1)) {
      local_8 = 0;
      local_10 = *(uint *)(param_1 + 0x118);
      do {
        if ((*(int *)(param_1 + 0x88) != 0) && (0 < *(int *)(param_1 + 0x118))) {
          if ((local_8 + *(int *)(param_1 + 0x14c) == *(int *)(param_1 + 0x11c)) &&
             (iVar10 = (**(code **)(**(int **)(param_1 + 0x110) + 0x14))(), 0 < iVar10)) {
            iVar10 = *(int *)(param_1 + 0x108);
            iVar5 = (*(int *)(iVar10 + 0x80) + *(int *)(param_1 + 0x80) + *(int *)(param_1 + 0x78))
                    - *(int *)(param_1 + 300);
            if (iVar5 < local_5c) {
              local_3c = iVar5;
            }
            iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),0);
            if (*(int *)(iVar5 + 0x24) + *(int *)(iVar10 + 0x84) + *(int *)(param_1 + 0x84) +
                ((*(int *)(param_1 + 0x11c) - *(int *)(param_1 + 0x14c)) + 1) *
                *(int *)(param_1 + 0x140) < local_58) {
              iVar10 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),0)
              ;
              local_38 = *(int *)(iVar10 + 0x24) +
                         *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84) +
                         ((*(int *)(param_1 + 0x11c) - *(int *)(param_1 + 0x14c)) + 1) *
                         *(int *)(param_1 + 0x140);
            }
            iVar10 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x13c),0);
            iVar10 = *(int *)(param_1 + 0x140) / *(int *)(iVar10 + 0x24);
            if (-1 < iVar10) {
              local_14 = iVar10 + 1;
              local_c = 0;
              do {
                iVar10 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                   (*(undefined4 *)(param_1 + 0x13c),0);
                iVar10 = (*(int *)(param_1 + 0x78) - *(int *)(param_1 + 300)) /
                         *(int *)(iVar10 + 0x20);
                if (-1 < iVar10) {
                  local_18 = iVar10 + 1;
                  iVar10 = 0;
                  do {
                    iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                      (*(undefined4 *)(param_1 + 0x134),1);
                    iVar5 = *(int *)(iVar5 + 0x24);
                    iVar6 = *(int *)(*(int *)(param_1 + 0x108) + 0x84);
                    iVar4 = *(int *)(param_1 + 0x84);
                    iVar7 = *(int *)(param_1 + 0x11c);
                    iVar13 = *(int *)(param_1 + 0x14c);
                    iVar11 = *(int *)(param_1 + 0x140);
                    iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                      (*(undefined4 *)(param_1 + 0x13c),0);
                    iVar13 = iVar5 + iVar6 + iVar4 + (iVar7 - iVar13) * iVar11 +
                             *(int *)(iVar8 + 0x24) * local_c;
                    iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                      (*(undefined4 *)(param_1 + 0x134),3);
                    iVar5 = *(int *)(iVar5 + 0x20);
                    iVar6 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
                    iVar4 = *(int *)(param_1 + 0x80);
                    iVar7 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                      (*(undefined4 *)(param_1 + 0x13c),0);
                    iVar7 = *(int *)(iVar7 + 0x20);
                    piVar3 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                              (*(undefined4 *)(param_1 + 0x13c),0);
                    (**(code **)(*piVar3 + 0x80))
                              (piVar3,iVar5 + iVar6 + iVar4 + iVar7 * iVar10,iVar13);
                    iVar10 = iVar10 + 1;
                    local_18 = local_18 + -1;
                  } while (local_18 != 0);
                }
                local_c = local_c + 1;
                local_14 = local_14 + -1;
              } while (local_14 != 0);
            }
          }
          iVar10 = (**(code **)(**(int **)(param_1 + 0x110) + 0x14))();
          if ((local_8 + *(int *)(param_1 + 0x14c) < iVar10) &&
             (iVar10 = (**(code **)(**(int **)(param_1 + 0x110) + 0x14))(), 0 < iVar10)) {
            iVar10 = (*(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80) +
                     *(int *)(param_1 + 0x78)) - *(int *)(param_1 + 300);
            if (iVar10 < local_5c) {
              local_3c = iVar10;
            }
            if (local_8 + *(int *)(param_1 + 0x14c) == *(int *)(param_1 + 0x11c)) {
              ImageLib_TImageLibraryFont_SetColor_23EDC2EF
                        (*(undefined4 *)(param_1 + 0x88),*(undefined4 *)(param_1 + 0x28));
            }
            (**(code **)(**(int **)(param_1 + 0x110) + 0xc))
                      (*(int **)(param_1 + 0x110),local_8 + *(int *)(param_1 + 0x14c),&local_68);
            puVar12 = local_54;
            uVar16 = local_68;
            iVar10 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),0);
            iVar10 = *(int *)(iVar10 + 0x24);
            iVar5 = *(int *)(*(int *)(param_1 + 0x108) + 0x84);
            iVar6 = *(int *)(param_1 + 0x84);
            iVar4 = *(int *)(param_1 + 0x98);
            iVar13 = *(int *)(param_1 + 0x140) * local_8;
            iVar7 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),3);
            ImageLib_TImageLibraryFont_SetText_23EDC2EF
                      (*(undefined4 *)(param_1 + 0x88),
                       *(int *)(iVar7 + 0x20) +
                       *(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80) +
                       *(int *)(param_1 + 0x94),iVar10 + iVar5 + iVar6 + iVar4 + iVar13,puVar12,
                       uVar16);
            if (local_8 + *(int *)(param_1 + 0x14c) == *(int *)(param_1 + 0x11c)) {
              ImageLib_TImageLibraryFont_SetColor_23EDC2EF
                        (*(undefined4 *)(param_1 + 0x88),*(undefined4 *)(param_1 + 0x24));
            }
            *(int *)(param_1 + 0x150) = local_8 + *(int *)(param_1 + 0x14c);
            iVar10 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),1);
            iVar10 = *(int *)(iVar10 + 0x24);
            iVar5 = *(int *)(param_1 + 0x108);
            iVar6 = *(int *)(iVar5 + 0x84);
            iVar4 = *(int *)(param_1 + 0x84);
            iVar11 = local_8 + 1;
            iVar7 = *(int *)(param_1 + 0x140);
            ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),1);
            iVar13 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),3);
            Classes_Rect_9923E97E
                      (*(int *)(iVar13 + 0x20) +
                       *(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80),
                       iVar10 + iVar6 + iVar4 + iVar11 * iVar7,
                       *(int *)(iVar5 + 0x80) + *(int *)(param_1 + 0x80) + *(int *)(param_1 + 0x78),
                       puVar12,uVar16);
            puVar12 = local_78;
            puVar14 = (undefined4 *)(param_1 + 0x154);
            for (iVar10 = 4; iVar10 != 0; iVar10 = iVar10 + -1) {
              *puVar14 = *puVar12;
              puVar12 = puVar12 + (uint)bVar15 * -2 + 1;
              puVar14 = puVar14 + (uint)bVar15 * -2 + 1;
            }
            if (*(short *)(param_1 + 0x126) != 0) {
              (**(code **)(param_1 + 0x124))(*(undefined4 *)(param_1 + 0x128),param_1);
            }
          }
          puVar12 = local_64;
          puVar14 = local_44;
          for (iVar10 = 4; iVar10 != 0; iVar10 = iVar10 + -1) {
            *puVar14 = *puVar12;
            puVar12 = puVar12 + (uint)bVar15 * -2 + 1;
            puVar14 = puVar14 + (uint)bVar15 * -2 + 1;
          }
        }
        local_8 = local_8 + 1;
        local_10 = local_10 - 1;
      } while (local_10 != 0);
    }
    puVar12 = local_54;
    iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),5);
    iVar10 = *(int *)(param_1 + 0x108);
    iVar5 = (*(int *)(iVar10 + 0x84) + *(int *)(param_1 + 0x84) + *(int *)(param_1 + 0x7c)) -
            *(int *)(iVar5 + 0x24);
    piVar3 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),5);
    (**(code **)(*piVar3 + 0x80))
              (piVar3,*(int *)(iVar10 + 0x80) + *(int *)(param_1 + 0x80),iVar5,puVar12);
    iVar10 = (*(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80) +
             *(int *)(param_1 + 0x78)) - *(int *)(param_1 + 300);
    if (iVar10 < local_5c) {
      local_3c = iVar10;
    }
    if (-1 < (int)(*(byte *)(param_1 + 0x9c) - 1)) {
      iVar10 = 0;
      local_10 = (uint)*(byte *)(param_1 + 0x9c);
      do {
        puVar12 = local_54;
        iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),5);
        iVar5 = *(int *)(iVar5 + 0x20);
        iVar6 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
        iVar4 = *(int *)(param_1 + 0x80);
        iVar7 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),6);
        iVar7 = *(int *)(iVar7 + 0x20);
        iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),6);
        iVar13 = *(int *)(*(int *)(param_1 + 0x108) + 0x84);
        iVar11 = *(int *)(param_1 + 0x84);
        iVar8 = *(int *)(param_1 + 0x7c);
        iVar9 = *(int *)(iVar9 + 0x24);
        piVar3 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(param_1 + 0x134),6);
        (**(code **)(*piVar3 + 0x80))
                  (piVar3,iVar5 + iVar6 + iVar4 + iVar7 * iVar10,(iVar13 + iVar11 + iVar8) - iVar9,
                   puVar12);
        iVar10 = iVar10 + 1;
        local_10 = local_10 - 1;
      } while (local_10 != 0);
    }
    puVar12 = local_54;
    iVar10 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),0);
    iVar10 = *(int *)(iVar10 + 0x20);
    iVar5 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
    iVar6 = *(int *)(param_1 + 0x80);
    iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),6);
    iVar4 = *(int *)(iVar4 + 0x20);
    bVar1 = *(byte *)(param_1 + 0x9c);
    iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),6);
    iVar7 = *(int *)(*(int *)(param_1 + 0x108) + 0x84);
    iVar13 = *(int *)(param_1 + 0x84);
    iVar11 = *(int *)(param_1 + 0x7c);
    iVar8 = *(int *)(iVar8 + 0x24);
    piVar3 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x134),6);
    (**(code **)(*piVar3 + 0x80))
              (piVar3,iVar10 + iVar5 + iVar6 + iVar4 * (uint)bVar1,(iVar7 + iVar13 + iVar11) - iVar8
               ,puVar12);
    puVar12 = local_64;
    puVar14 = local_44;
    for (iVar10 = 4; iVar10 != 0; iVar10 = iVar10 + -1) {
      *puVar14 = *puVar12;
      puVar12 = puVar12 + (uint)bVar15 * -2 + 1;
      puVar14 = puVar14 + (uint)bVar15 * -2 + 1;
    }
    puVar2 = puStack_88;
    if (*(short *)(param_1 + 0x32) != 0) {
      (**(code **)(param_1 + 0x30))(*(undefined4 *)(param_1 + 0x34),param_1,local_54);
      puVar2 = puStack_88;
    }
  }
  puStack_88 = puVar2;
  puVar2 = puStack_88;
  *in_FS_OFFSET = uStack_90;
  puStack_88 = &LAB_00413896;
  puStack_8c = (undefined1 *)0x41388e;
  System__LStrClr_40929B27(&local_68,uStack_90,puVar2);
  return;
}
