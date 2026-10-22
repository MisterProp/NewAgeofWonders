void AoWComboBox_TAOWComboBox_Draw_23EDC2EF(int param_1,undefined4 *param_2)
{
  undefined1 *puVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  uint uVar9;
  undefined4 *puVar10;
  int iVar11;
  undefined4 *puVar12;
  undefined4 *in_FS_OFFSET;
  byte bVar13;
  undefined4 uVar14;
  undefined4 uStack_8c;
  undefined1 *puStack_88;
  undefined1 *puStack_84;
  undefined4 local_74 [4];
  undefined4 local_64;
  undefined4 local_60 [2];
  int local_58;
  int local_54;
  undefined4 local_50 [4];
  undefined4 local_40 [2];
  int local_38;
  int local_34;
  int local_14;
  int local_10;
  int local_c;
  int local_8;
                    /* 0xeb64  229  AoWComboBox.TAOWComboBox.Draw@23EDC2EF */
  bVar13 = 0;
  puStack_84 = &stack0xfffffffc;
  local_64 = 0;
  puVar10 = local_50;
  for (iVar8 = 0xf; iVar8 != 0; iVar8 = iVar8 + -1) {
    *puVar10 = *param_2;
    param_2 = param_2 + 1;
    puVar10 = puVar10 + 1;
  }
  puStack_88 = &LAB_0040f790;
  uStack_8c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_8c;
  puVar1 = &stack0xfffffffc;
  if ((*(char *)(param_1 + 0x2c) == '\x01') &&
     (puVar1 = &stack0xfffffffc, *(char *)(param_1 + 0x69) == '\x01')) {
    ImageLib_TImageLibraryFont_SetColor_23EDC2EF
              (*(undefined4 *)(param_1 + 0x88),*(undefined4 *)(param_1 + 0x24));
    puVar10 = local_40;
    puVar12 = local_60;
    for (iVar8 = 4; iVar8 != 0; iVar8 = iVar8 + -1) {
      *puVar12 = *puVar10;
      puVar10 = puVar10 + (uint)bVar13 * -2 + 1;
      puVar12 = puVar12 + (uint)bVar13 * -2 + 1;
    }
    piVar2 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                              (*(undefined4 *)(param_1 + 0x148),0,0,local_50);
    (**(code **)(*piVar2 + 0x80))
              (piVar2,*(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80),
               *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84));
    uVar9 = (uint)*(byte *)(param_1 + 0x9c);
    if (-1 < (int)(uVar9 - 1)) {
      local_8 = 0;
      do {
        puVar10 = local_50;
        iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x148),0);
        iVar8 = *(int *)(iVar8 + 0x20);
        iVar4 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
        iVar7 = *(int *)(param_1 + 0x80);
        iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x148),1);
        iVar8 = iVar8 + iVar4 + iVar7 + *(int *)(iVar3 + 0x20) * local_8;
        piVar2 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(param_1 + 0x148),1);
        (**(code **)(*piVar2 + 0x80))
                  (piVar2,iVar8,
                   *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84),puVar10);
        local_8 = local_8 + 1;
        uVar9 = uVar9 - 1;
      } while (uVar9 != 0);
    }
    iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x148),0);
    iVar8 = *(int *)(iVar8 + 0x20);
    iVar4 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
    iVar7 = *(int *)(param_1 + 0x80);
    iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x148),1);
    if ((int)(iVar8 + iVar4 + iVar7 + *(int *)(iVar3 + 0x20) * (uint)*(byte *)(param_1 + 0x9c) +
             *(int *)(param_1 + 0xa0)) < local_58) {
      iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x148),0);
      iVar8 = *(int *)(iVar8 + 0x20);
      iVar4 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
      iVar7 = *(int *)(param_1 + 0x80);
      iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x148),1);
      local_38 = iVar8 + iVar4 + iVar7 + *(int *)(iVar3 + 0x20) * (uint)*(byte *)(param_1 + 0x9c) +
                 *(int *)(param_1 + 0xa0);
    }
    puVar10 = local_50;
    iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x148),0);
    iVar8 = *(int *)(iVar8 + 0x20);
    iVar4 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
    iVar7 = *(int *)(param_1 + 0x80);
    iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x148),1);
    iVar8 = iVar8 + iVar4 + iVar7 + *(int *)(iVar3 + 0x20) * (uint)*(byte *)(param_1 + 0x9c);
    piVar2 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x148),1);
    (**(code **)(*piVar2 + 0x80))
              (piVar2,iVar8,*(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84),
               puVar10);
    iVar8 = (**(code **)(**(int **)(param_1 + 0x118) + 0x14))();
    if ((*(int *)(param_1 + 0x128) < iVar8) && (*(int *)(param_1 + 0x88) != 0)) {
      (**(code **)(**(int **)(param_1 + 0x118) + 0xc))
                (*(int **)(param_1 + 0x118),*(undefined4 *)(param_1 + 0x128),&local_64);
      ImageLib_TImageLibraryFont_SetText_23EDC2EF
                (*(undefined4 *)(param_1 + 0x88),
                 *(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80) +
                 *(int *)(param_1 + 0x94),
                 *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84) +
                 *(int *)(param_1 + 0x98),local_50,local_64);
    }
    puVar10 = local_60;
    puVar12 = local_40;
    for (iVar8 = 4; iVar8 != 0; iVar8 = iVar8 + -1) {
      *puVar12 = *puVar10;
      puVar10 = puVar10 + (uint)bVar13 * -2 + 1;
      puVar12 = puVar12 + (uint)bVar13 * -2 + 1;
    }
    iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x148),0,0,local_50)
    ;
    iVar8 = *(int *)(iVar8 + 0x20);
    iVar4 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
    iVar7 = *(int *)(param_1 + 0x80);
    iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x148),1);
    iVar8 = iVar8 + iVar4 + iVar7 + *(int *)(iVar3 + 0x20) * (uint)*(byte *)(param_1 + 0x9c) +
            *(int *)(param_1 + 0xa0);
    piVar2 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x148),2);
    (**(code **)(*piVar2 + 0x80))
              (piVar2,iVar8,*(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84));
    if (*(int *)(param_1 + 0x114) != 0) {
      iVar8 = *(int *)(param_1 + 0x108);
      iVar4 = (*(int *)(iVar8 + 0x80) + *(int *)(param_1 + 0x80) + *(int *)(param_1 + 0x78)) -
              *(int *)(param_1 + 0x140);
      if (iVar4 < local_58) {
        local_38 = iVar4;
      }
      iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x148),0);
      if (*(int *)(iVar4 + 0x24) + *(int *)(iVar8 + 0x84) + *(int *)(param_1 + 0x84) +
          *(int *)(param_1 + 0x158) * *(int *)(param_1 + 0x120) < local_54) {
        iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x148),0);
        local_34 = *(int *)(iVar8 + 0x24) +
                   *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84) +
                   *(int *)(param_1 + 0x158) * *(int *)(param_1 + 0x120);
      }
      iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x150),0);
      iVar8 = (*(int *)(param_1 + 0x158) * *(int *)(param_1 + 0x120)) / *(int *)(iVar8 + 0x24);
      if (-1 < iVar8) {
        iVar8 = iVar8 + 1;
        local_10 = 0;
        do {
          iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x150),0);
          iVar4 = (*(int *)(param_1 + 0x78) - *(int *)(param_1 + 0x140)) / *(int *)(iVar4 + 0x20);
          if (-1 < iVar4) {
            local_14 = iVar4 + 1;
            local_c = 0;
            do {
              iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x148),0);
              iVar4 = *(int *)(iVar4 + 0x24);
              iVar7 = *(int *)(*(int *)(param_1 + 0x108) + 0x84);
              iVar3 = *(int *)(param_1 + 0x84);
              iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x150),0);
              iVar11 = iVar4 + iVar7 + iVar3 + *(int *)(iVar5 + 0x24) * local_10;
              iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x14c),3);
              iVar4 = *(int *)(iVar4 + 0x20);
              iVar7 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
              iVar3 = *(int *)(param_1 + 0x80);
              iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x150),0);
              iVar4 = iVar4 + iVar7 + iVar3 + *(int *)(iVar5 + 0x20) * local_c;
              piVar2 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                        (*(undefined4 *)(param_1 + 0x150),0);
              (**(code **)(*piVar2 + 0x80))(piVar2,iVar4,iVar11);
              local_c = local_c + 1;
              local_14 = local_14 + -1;
            } while (local_14 != 0);
          }
          local_10 = local_10 + 1;
          iVar8 = iVar8 + -1;
        } while (iVar8 != 0);
      }
      if (-1 < *(int *)(param_1 + 300)) {
        iVar8 = *(int *)(param_1 + 0x108);
        iVar4 = (*(int *)(iVar8 + 0x80) + *(int *)(param_1 + 0x80) + *(int *)(param_1 + 0x78)) -
                *(int *)(param_1 + 0x140);
        if (iVar4 < local_58) {
          local_38 = iVar4;
        }
        iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x148),0);
        if (*(int *)(iVar4 + 0x24) + *(int *)(iVar8 + 0x84) + *(int *)(param_1 + 0x84) +
            (*(int *)(param_1 + 300) + 1) * *(int *)(param_1 + 0x158) < local_54) {
          iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x148),0);
          local_34 = *(int *)(iVar8 + 0x24) +
                     *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84) +
                     (*(int *)(param_1 + 300) + 1) * *(int *)(param_1 + 0x158);
        }
        iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x154),0);
        iVar8 = *(int *)(param_1 + 0x158) / *(int *)(iVar8 + 0x24);
        if (-1 < iVar8) {
          iVar8 = iVar8 + 1;
          local_10 = 0;
          do {
            iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x150),0);
            iVar4 = (*(int *)(param_1 + 0x78) - *(int *)(param_1 + 0x140)) / *(int *)(iVar4 + 0x20);
            if (-1 < iVar4) {
              local_14 = iVar4 + 1;
              local_c = 0;
              do {
                iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(param_1 + 0x148),0);
                iVar4 = *(int *)(iVar4 + 0x24);
                iVar7 = *(int *)(*(int *)(param_1 + 0x108) + 0x84);
                iVar3 = *(int *)(param_1 + 0x84);
                iVar5 = *(int *)(param_1 + 0x158);
                iVar11 = *(int *)(param_1 + 300);
                iVar6 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(param_1 + 0x154),0);
                iVar11 = iVar4 + iVar7 + iVar3 + iVar5 * iVar11 + *(int *)(iVar6 + 0x24) * local_10;
                iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(param_1 + 0x14c),3);
                iVar4 = *(int *)(iVar4 + 0x20);
                iVar7 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
                iVar3 = *(int *)(param_1 + 0x80);
                iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(param_1 + 0x154),0);
                iVar4 = iVar4 + iVar7 + iVar3 + *(int *)(iVar5 + 0x20) * local_c;
                piVar2 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                          (*(undefined4 *)(param_1 + 0x154),0);
                (**(code **)(*piVar2 + 0x80))(piVar2,iVar4,iVar11);
                local_c = local_c + 1;
                local_14 = local_14 + -1;
              } while (local_14 != 0);
            }
            local_10 = local_10 + 1;
            iVar8 = iVar8 + -1;
          } while (iVar8 != 0);
        }
      }
      puVar10 = local_60;
      puVar12 = local_40;
      for (iVar8 = 4; iVar8 != 0; iVar8 = iVar8 + -1) {
        *puVar12 = *puVar10;
        puVar10 = puVar10 + (uint)bVar13 * -2 + 1;
        puVar12 = puVar12 + (uint)bVar13 * -2 + 1;
      }
      iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x148),0);
      if (*(int *)(iVar8 + 0x24) +
          *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84) +
          *(int *)(param_1 + 0x158) * *(int *)(param_1 + 0x120) < local_54) {
        iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x148),0);
        local_34 = *(int *)(iVar8 + 0x24) +
                   *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84) +
                   *(int *)(param_1 + 0x158) * *(int *)(param_1 + 0x120);
      }
      iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x14c),3);
      iVar8 = (*(int *)(param_1 + 0x158) * *(int *)(param_1 + 0x120)) / *(int *)(iVar8 + 0x24);
      if (-1 < iVar8) {
        iVar8 = iVar8 + 1;
        local_10 = 0;
        do {
          puVar10 = local_50;
          iVar7 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x148),0);
          iVar4 = *(int *)(param_1 + 0x108);
          iVar7 = *(int *)(iVar7 + 0x24) + *(int *)(iVar4 + 0x84) + *(int *)(param_1 + 0x84);
          iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x14c),3);
          iVar7 = iVar7 + *(int *)(iVar3 + 0x24) * local_10;
          piVar2 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                    (*(undefined4 *)(param_1 + 0x14c),3);
          (**(code **)(*piVar2 + 0x80))
                    (piVar2,*(int *)(iVar4 + 0x80) + *(int *)(param_1 + 0x80),iVar7,puVar10);
          local_10 = local_10 + 1;
          iVar8 = iVar8 + -1;
        } while (iVar8 != 0);
      }
      iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x14c),4);
      iVar8 = (*(int *)(param_1 + 0x158) * *(int *)(param_1 + 0x120)) / *(int *)(iVar8 + 0x24);
      if (-1 < iVar8) {
        iVar8 = iVar8 + 1;
        local_10 = 0;
        do {
          if (*(char *)(param_1 + 0x144) == '\0') {
            puVar10 = local_50;
            iVar7 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x148),0);
            iVar4 = *(int *)(param_1 + 0x108);
            iVar7 = *(int *)(iVar7 + 0x24) + *(int *)(iVar4 + 0x84) + *(int *)(param_1 + 0x84);
            iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x14c),4);
            iVar7 = iVar7 + *(int *)(iVar3 + 0x24) * local_10;
            piVar2 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                      (*(undefined4 *)(param_1 + 0x14c),4);
            (**(code **)(*piVar2 + 0x80))
                      (piVar2,(*(int *)(iVar4 + 0x80) + *(int *)(param_1 + 0x80) +
                              *(int *)(param_1 + 0x78)) - *(int *)(param_1 + 0x140),iVar7,puVar10);
          }
          local_10 = local_10 + 1;
          iVar8 = iVar8 + -1;
        } while (iVar8 != 0);
      }
      puVar10 = local_60;
      puVar12 = local_40;
      for (iVar8 = 4; iVar8 != 0; iVar8 = iVar8 + -1) {
        *puVar12 = *puVar10;
        puVar10 = puVar10 + (uint)bVar13 * -2 + 1;
        puVar12 = puVar12 + (uint)bVar13 * -2 + 1;
      }
      iVar8 = *(int *)(param_1 + 0x120);
      if (-1 < iVar8 + -1) {
        local_8 = 0;
        do {
          if ((*(int *)(param_1 + 0x88) != 0) &&
             (iVar4 = (**(code **)(**(int **)(param_1 + 0x118) + 0x14))(), 0 < iVar4)) {
            iVar4 = (*(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80) +
                    *(int *)(param_1 + 0x78)) - *(int *)(param_1 + 0x140);
            if (iVar4 < local_58) {
              local_38 = iVar4;
            }
            if (local_8 + *(int *)(param_1 + 0x124) == *(int *)(param_1 + 300)) {
              ImageLib_TImageLibraryFont_SetColor_23EDC2EF
                        (*(undefined4 *)(param_1 + 0x88),*(undefined4 *)(param_1 + 0x28));
            }
            (**(code **)(**(int **)(param_1 + 0x118) + 0xc))
                      (*(int **)(param_1 + 0x118),local_8 + *(int *)(param_1 + 0x124),&local_64);
            puVar10 = local_50;
            uVar14 = local_64;
            iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x148),0);
            ImageLib_TImageLibraryFont_SetText_23EDC2EF
                      (*(undefined4 *)(param_1 + 0x88),
                       *(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80) +
                       *(int *)(param_1 + 0x94),
                       *(int *)(iVar4 + 0x24) +
                       *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84) +
                       *(int *)(param_1 + 0x98) + *(int *)(param_1 + 0x158) * local_8,puVar10,uVar14
                      );
            if (local_8 + *(int *)(param_1 + 0x124) == *(int *)(param_1 + 300)) {
              ImageLib_TImageLibraryFont_SetColor_23EDC2EF
                        (*(undefined4 *)(param_1 + 0x88),*(undefined4 *)(param_1 + 0x24));
            }
            *(int *)(param_1 + 0x15c) = local_8 + *(int *)(param_1 + 0x124);
            ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x148),1);
            iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x14c),3);
            Classes_Rect_9923E97E
                      (*(int *)(iVar4 + 0x20) +
                       *(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80),
                       *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84) +
                       *(int *)(param_1 + 0x158) + local_8 * *(int *)(param_1 + 0x158),
                       *(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80) +
                       *(int *)(param_1 + 0x78),puVar10,uVar14);
            puVar10 = local_74;
            puVar12 = (undefined4 *)(param_1 + 0x160);
            for (iVar4 = 4; iVar4 != 0; iVar4 = iVar4 + -1) {
              *puVar12 = *puVar10;
              puVar10 = puVar10 + (uint)bVar13 * -2 + 1;
              puVar12 = puVar12 + (uint)bVar13 * -2 + 1;
            }
            if (*(short *)(param_1 + 0x13a) != 0) {
              (**(code **)(param_1 + 0x138))(*(undefined4 *)(param_1 + 0x13c),param_1);
            }
          }
          local_8 = local_8 + 1;
          iVar8 = iVar8 + -1;
        } while (iVar8 != 0);
      }
      puVar10 = local_60;
      puVar12 = local_40;
      for (iVar8 = 4; iVar8 != 0; iVar8 = iVar8 + -1) {
        *puVar12 = *puVar10;
        puVar10 = puVar10 + (uint)bVar13 * -2 + 1;
        puVar12 = puVar12 + (uint)bVar13 * -2 + 1;
      }
      iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                        (*(undefined4 *)(param_1 + 0x14c),5,0,local_50);
      iVar8 = *(int *)(param_1 + 0x108);
      iVar4 = (*(int *)(iVar8 + 0x84) + *(int *)(param_1 + 0x84) + *(int *)(param_1 + 0x7c)) -
              *(int *)(iVar4 + 0x24);
      piVar2 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x14c),5);
      (**(code **)(*piVar2 + 0x80))(piVar2,*(int *)(iVar8 + 0x80) + *(int *)(param_1 + 0x80),iVar4);
      if (*(char *)(param_1 + 0x144) == '\0') {
        puVar10 = local_50;
        iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x14c),7);
        iVar8 = *(int *)(param_1 + 0x108);
        iVar4 = (*(int *)(iVar8 + 0x84) + *(int *)(param_1 + 0x84) + *(int *)(param_1 + 0x7c)) -
                *(int *)(iVar4 + 0x24);
        piVar2 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(param_1 + 0x14c),7);
        (**(code **)(*piVar2 + 0x80))
                  (piVar2,(*(int *)(iVar8 + 0x80) + *(int *)(param_1 + 0x80) +
                          *(int *)(param_1 + 0x78)) - *(int *)(param_1 + 0x140),iVar4,puVar10);
      }
      iVar8 = *(int *)(param_1 + 0x78);
      iVar4 = *(int *)(param_1 + 0x140);
      iVar7 = (*(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80) + iVar8) -
              iVar4;
      if (iVar7 < local_58) {
        local_38 = iVar7;
      }
      iVar7 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x14c),6);
      iVar8 = (iVar8 - iVar4) / *(int *)(iVar7 + 0x20);
      if (-1 < iVar8) {
        iVar8 = iVar8 + 1;
        local_c = 0;
        do {
          puVar10 = local_50;
          iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x148),0);
          iVar4 = *(int *)(iVar4 + 0x20);
          iVar7 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
          iVar3 = *(int *)(param_1 + 0x80);
          iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x14c),6);
          iVar7 = iVar4 + iVar7 + iVar3 + *(int *)(iVar5 + 0x20) * local_c;
          iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x14c),6);
          iVar4 = (*(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84) +
                  *(int *)(param_1 + 0x7c)) - *(int *)(iVar4 + 0x24);
          piVar2 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                    (*(undefined4 *)(param_1 + 0x14c),6);
          (**(code **)(*piVar2 + 0x80))(piVar2,iVar7,iVar4,puVar10);
          local_c = local_c + 1;
          iVar8 = iVar8 + -1;
        } while (iVar8 != 0);
      }
      puVar10 = local_60;
      puVar12 = local_40;
      for (iVar8 = 4; iVar8 != 0; iVar8 = iVar8 + -1) {
        *puVar12 = *puVar10;
        puVar10 = puVar10 + (uint)bVar13 * -2 + 1;
        puVar12 = puVar12 + (uint)bVar13 * -2 + 1;
      }
    }
    puVar1 = puStack_84;
    if (*(short *)(param_1 + 0x32) != 0) {
      (**(code **)(param_1 + 0x30))(*(undefined4 *)(param_1 + 0x34),param_1,local_50);
      puVar1 = puStack_84;
    }
  }
  puStack_84 = puVar1;
  puVar1 = puStack_84;
  *in_FS_OFFSET = uStack_8c;
  puStack_84 = &LAB_0040f797;
  puStack_88 = (undefined1 *)0x40f78f;
  System__LStrClr_40929B27(&local_64,uStack_8c,puVar1);
  return;
}
