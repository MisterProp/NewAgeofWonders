// /AoWInterface.dpl : AoWTabPanel.TAOWTabPanel.Draw@23EDC2EF

void AoWTabPanel_TAOWTabPanel_Draw_23EDC2EF(int param_1,undefined4 *param_2)

{
  byte bVar1;
  byte bVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined1 *puVar7;
  int iVar8;
  int iVar9;
  int *piVar10;
  int iVar11;
  int iVar12;
  int iVar13;
  uint uVar14;
  int iVar15;
  undefined4 *puVar16;
  int *piVar17;
  undefined4 *in_FS_OFFSET;
  byte bVar18;
  undefined4 uStack_7c;
  undefined1 *puStack_78;
  undefined1 *puStack_74;
  undefined4 local_64;
  undefined4 local_60;
  int local_5c [4];
  undefined4 local_4c [4];
  int local_3c [11];
  uint local_10;
  int local_c;
  int local_8;
  
                    /* 0x141d8  314  AoWTabPanel.TAOWTabPanel.Draw@23EDC2EF */
  bVar18 = 0;
  puStack_74 = &stack0xfffffffc;
  local_60 = 0;
  local_64 = 0;
  puVar16 = local_4c;
  for (iVar13 = 0xf; iVar13 != 0; iVar13 = iVar13 + -1) {
    *puVar16 = *param_2;
    param_2 = param_2 + 1;
    puVar16 = puVar16 + 1;
  }
  puStack_78 = &LAB_00414d47;
  uStack_7c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_7c;
  puVar7 = &stack0xfffffffc;
  if ((*(char *)(param_1 + 0x2c) == '\x01') &&
     (puVar7 = &stack0xfffffffc, *(char *)(param_1 + 0x69) == '\x01')) {
    piVar10 = local_3c;
    piVar17 = local_5c;
    for (iVar13 = 4; iVar13 != 0; iVar13 = iVar13 + -1) {
      *piVar17 = *piVar10;
      piVar10 = piVar10 + 1;
      piVar17 = piVar17 + 1;
    }
    iVar13 = (**(code **)(**(int **)(param_1 + 0x110) + 0x14))();
    if (-1 < iVar13 + -1) {
      iVar15 = 0;
      local_c = iVar13;
      do {
        if (iVar15 == *(int *)(param_1 + 0x114)) {
          puVar16 = local_4c;
          iVar13 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),0);
          iVar13 = *(int *)(iVar13 + 0x20);
          iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),1);
          iVar8 = *(int *)(iVar8 + 0x20);
          bVar1 = *(byte *)(param_1 + 0x9c);
          iVar3 = *(int *)(param_1 + 0xa0);
          iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),2);
          iVar9 = *(int *)(iVar9 + 0x20);
          iVar4 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
          iVar5 = *(int *)(param_1 + 0x80);
          piVar10 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                     (*(undefined4 *)(param_1 + 0x118),0);
          (**(code **)(*piVar10 + 0x80))
                    (piVar10,(iVar13 + iVar8 * (uint)bVar1 + iVar3 + iVar9) * iVar15 + iVar4 + iVar5
                     ,*(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84),puVar16)
          ;
          if (-1 < (int)(*(byte *)(param_1 + 0x9c) - 1)) {
            local_8 = 0;
            local_10 = (uint)*(byte *)(param_1 + 0x9c);
            do {
              iVar13 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),0)
              ;
              iVar13 = *(int *)(iVar13 + 0x20);
              iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),1);
              iVar8 = *(int *)(iVar8 + 0x20);
              bVar1 = *(byte *)(param_1 + 0x9c);
              iVar3 = *(int *)(param_1 + 0xa0);
              iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),2);
              iVar9 = *(int *)(iVar9 + 0x20);
              iVar4 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
              iVar5 = *(int *)(param_1 + 0x80);
              iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),0)
              ;
              iVar11 = *(int *)(iVar11 + 0x20);
              iVar12 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),1)
              ;
              iVar13 = (iVar13 + iVar8 * (uint)bVar1 + iVar3 + iVar9) * iVar15 + iVar4 + iVar5 +
                       iVar11 + *(int *)(iVar12 + 0x20) * local_8;
              piVar10 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                         (*(undefined4 *)(param_1 + 0x118),1);
              (**(code **)(*piVar10 + 0x80))
                        (piVar10,iVar13,
                         *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84));
              local_8 = local_8 + 1;
              local_10 = local_10 - 1;
            } while (local_10 != 0);
          }
          iVar13 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),0);
          iVar13 = *(int *)(iVar13 + 0x20);
          iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),1);
          iVar8 = *(int *)(iVar8 + 0x20);
          bVar1 = *(byte *)(param_1 + 0x9c);
          iVar3 = *(int *)(param_1 + 0xa0);
          iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),2);
          iVar9 = *(int *)(iVar9 + 0x20);
          iVar4 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
          iVar5 = *(int *)(param_1 + 0x80);
          iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),0);
          iVar11 = *(int *)(iVar11 + 0x20);
          iVar12 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),1);
          if ((int)((iVar13 + iVar8 * (uint)bVar1 + iVar3 + iVar9) * iVar15 + iVar4 + iVar5 + iVar11
                    + *(int *)(iVar12 + 0x20) * (uint)*(byte *)(param_1 + 0x9c) +
                   *(int *)(param_1 + 0xa0)) < local_5c[2]) {
            iVar13 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),0);
            iVar13 = *(int *)(iVar13 + 0x20);
            iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),1);
            iVar8 = *(int *)(iVar8 + 0x20);
            bVar1 = *(byte *)(param_1 + 0x9c);
            iVar3 = *(int *)(param_1 + 0xa0);
            iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),2);
            iVar9 = *(int *)(iVar9 + 0x20);
            iVar4 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
            iVar5 = *(int *)(param_1 + 0x80);
            iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),0);
            iVar11 = *(int *)(iVar11 + 0x20);
            iVar12 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),1);
            local_3c[2] = (iVar13 + iVar8 * (uint)bVar1 + iVar3 + iVar9) * iVar15 + iVar4 + iVar5 +
                          iVar11 + *(int *)(iVar12 + 0x20) * (uint)*(byte *)(param_1 + 0x9c) +
                          *(int *)(param_1 + 0xa0);
          }
          iVar13 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),0);
          iVar13 = *(int *)(iVar13 + 0x20);
          iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),1);
          iVar8 = *(int *)(iVar8 + 0x20);
          bVar1 = *(byte *)(param_1 + 0x9c);
          iVar3 = *(int *)(param_1 + 0xa0);
          iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),2);
          iVar9 = *(int *)(iVar9 + 0x20);
          iVar4 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
          iVar5 = *(int *)(param_1 + 0x80);
          iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),0);
          iVar11 = *(int *)(iVar11 + 0x20);
          iVar12 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),1);
          iVar13 = (iVar13 + iVar8 * (uint)bVar1 + iVar3 + iVar9) * iVar15 + iVar4 + iVar5 + iVar11
                   + *(int *)(iVar12 + 0x20) * (uint)*(byte *)(param_1 + 0x9c);
          piVar10 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                     (*(undefined4 *)(param_1 + 0x118),1);
          (**(code **)(*piVar10 + 0x80))
                    (piVar10,iVar13,
                     *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84));
          piVar10 = local_5c;
          piVar17 = local_3c;
          for (iVar13 = 4; iVar13 != 0; iVar13 = iVar13 + -1) {
            *piVar17 = *piVar10;
            piVar10 = piVar10 + (uint)bVar18 * -2 + 1;
            piVar17 = piVar17 + (uint)bVar18 * -2 + 1;
          }
          iVar13 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                             (*(undefined4 *)(param_1 + 0x118),0,0,local_4c);
          iVar13 = *(int *)(iVar13 + 0x20);
          iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),1);
          iVar8 = *(int *)(iVar8 + 0x20);
          bVar1 = *(byte *)(param_1 + 0x9c);
          iVar3 = *(int *)(param_1 + 0xa0);
          iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),2);
          iVar9 = *(int *)(iVar9 + 0x20);
          iVar4 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
          iVar5 = *(int *)(param_1 + 0x80);
          iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),0);
          iVar11 = *(int *)(iVar11 + 0x20);
          iVar12 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),1);
          iVar12 = *(int *)(iVar12 + 0x20);
          bVar2 = *(byte *)(param_1 + 0x9c);
          iVar6 = *(int *)(param_1 + 0xa0);
          piVar10 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                     (*(undefined4 *)(param_1 + 0x118),2);
          (**(code **)(*piVar10 + 0x80))
                    (piVar10,(iVar13 + iVar8 * (uint)bVar1 + iVar3 + iVar9) * iVar15 + iVar4 + iVar5
                             + iVar11 + iVar12 * (uint)bVar2 + iVar6,
                     *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84));
          if (*(int *)(param_1 + 0x88) != 0) {
            ImageLib_TImageLibraryFont_SetColor_23EDC2EF
                      (*(int *)(param_1 + 0x88),*(undefined4 *)(param_1 + 0x28));
            (**(code **)(**(int **)(param_1 + 0x110) + 0xc))
                      (*(int **)(param_1 + 0x110),iVar15,&local_60);
            iVar13 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),0);
            iVar13 = *(int *)(iVar13 + 0x20);
            iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),1);
            iVar8 = *(int *)(iVar8 + 0x20);
            bVar1 = *(byte *)(param_1 + 0x9c);
            iVar3 = *(int *)(param_1 + 0xa0);
            iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),2);
            iVar9 = *(int *)(iVar9 + 0x20);
            iVar4 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
            iVar5 = *(int *)(param_1 + 0x80);
            iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),0);
            iVar11 = *(int *)(iVar11 + 0x20);
            iVar12 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),1);
            iVar12 = iVar11 + *(int *)(iVar12 + 0x20) * (uint)*(byte *)(param_1 + 0x9c) +
                     *(int *)(param_1 + 0xa0);
            iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),2);
            uVar14 = iVar12 + *(int *)(iVar11 + 0x20);
            iVar11 = (int)uVar14 >> 1;
            if (iVar11 < 0) {
              iVar11 = iVar11 + (uint)((uVar14 & 1) != 0);
            }
            iVar11 = (iVar13 + iVar8 * (uint)bVar1 + iVar3 + iVar9) * iVar15 + iVar4 + iVar5 +
                     iVar11;
            (**(code **)(**(int **)(param_1 + 0x110) + 0xc))
                      (*(int **)(param_1 + 0x110),iVar15,&local_64);
            uVar14 = ImageLib_TImageLibraryFont_GetTextWidth_23EDC2EF
                               (*(undefined4 *)(param_1 + 0x88),local_64);
            iVar13 = (int)uVar14 >> 1;
            if (iVar13 < 0) {
              iVar13 = iVar13 + (uint)((uVar14 & 1) != 0);
            }
            ImageLib_TImageLibraryFont_SetText_23EDC2EF
                      (*(undefined4 *)(param_1 + 0x88),iVar11 - iVar13,
                       *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84) +
                       *(int *)(param_1 + 0x11c));
          }
        }
        else {
          puVar16 = local_4c;
          iVar13 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),0);
          iVar13 = *(int *)(iVar13 + 0x20);
          iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),1);
          iVar8 = *(int *)(iVar8 + 0x20);
          bVar1 = *(byte *)(param_1 + 0x9c);
          iVar3 = *(int *)(param_1 + 0xa0);
          iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),2);
          iVar9 = *(int *)(iVar9 + 0x20);
          iVar4 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
          iVar5 = *(int *)(param_1 + 0x80);
          piVar10 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                     (*(undefined4 *)(param_1 + 0x118),3);
          (**(code **)(*piVar10 + 0x80))
                    (piVar10,(iVar13 + iVar8 * (uint)bVar1 + iVar3 + iVar9) * iVar15 + iVar4 + iVar5
                     ,*(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84),puVar16)
          ;
          if (-1 < (int)(*(byte *)(param_1 + 0x9c) - 1)) {
            local_8 = 0;
            local_10 = (uint)*(byte *)(param_1 + 0x9c);
            do {
              iVar13 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),0)
              ;
              iVar13 = *(int *)(iVar13 + 0x20);
              iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),1);
              iVar8 = *(int *)(iVar8 + 0x20);
              bVar1 = *(byte *)(param_1 + 0x9c);
              iVar3 = *(int *)(param_1 + 0xa0);
              iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),2);
              iVar9 = *(int *)(iVar9 + 0x20);
              iVar4 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
              iVar5 = *(int *)(param_1 + 0x80);
              iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),0)
              ;
              iVar11 = *(int *)(iVar11 + 0x20);
              iVar12 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),1)
              ;
              iVar13 = (iVar13 + iVar8 * (uint)bVar1 + iVar3 + iVar9) * iVar15 + iVar4 + iVar5 +
                       iVar11 + *(int *)(iVar12 + 0x20) * local_8;
              piVar10 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                         (*(undefined4 *)(param_1 + 0x118),4);
              (**(code **)(*piVar10 + 0x80))
                        (piVar10,iVar13,
                         *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84));
              local_8 = local_8 + 1;
              local_10 = local_10 - 1;
            } while (local_10 != 0);
          }
          iVar13 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),0);
          iVar13 = *(int *)(iVar13 + 0x20);
          iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),1);
          iVar8 = *(int *)(iVar8 + 0x20);
          bVar1 = *(byte *)(param_1 + 0x9c);
          iVar3 = *(int *)(param_1 + 0xa0);
          iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),2);
          iVar9 = *(int *)(iVar9 + 0x20);
          iVar4 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
          iVar5 = *(int *)(param_1 + 0x80);
          iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),0);
          iVar11 = *(int *)(iVar11 + 0x20);
          iVar12 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),1);
          if ((int)((iVar13 + iVar8 * (uint)bVar1 + iVar3 + iVar9) * iVar15 + iVar4 + iVar5 + iVar11
                    + *(int *)(iVar12 + 0x20) * (uint)*(byte *)(param_1 + 0x9c) +
                   *(int *)(param_1 + 0xa0)) < local_5c[2]) {
            iVar13 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),0);
            iVar13 = *(int *)(iVar13 + 0x20);
            iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),1);
            iVar8 = *(int *)(iVar8 + 0x20);
            bVar1 = *(byte *)(param_1 + 0x9c);
            iVar3 = *(int *)(param_1 + 0xa0);
            iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),2);
            iVar9 = *(int *)(iVar9 + 0x20);
            iVar4 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
            iVar5 = *(int *)(param_1 + 0x80);
            iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),0);
            iVar11 = *(int *)(iVar11 + 0x20);
            iVar12 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),1);
            local_3c[2] = (iVar13 + iVar8 * (uint)bVar1 + iVar3 + iVar9) * iVar15 + iVar4 + iVar5 +
                          iVar11 + *(int *)(iVar12 + 0x20) * (uint)*(byte *)(param_1 + 0x9c) +
                          *(int *)(param_1 + 0xa0);
          }
          iVar13 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),0);
          iVar13 = *(int *)(iVar13 + 0x20);
          iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),1);
          iVar8 = *(int *)(iVar8 + 0x20);
          bVar1 = *(byte *)(param_1 + 0x9c);
          iVar3 = *(int *)(param_1 + 0xa0);
          iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),2);
          iVar9 = *(int *)(iVar9 + 0x20);
          iVar4 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
          iVar5 = *(int *)(param_1 + 0x80);
          iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),0);
          iVar11 = *(int *)(iVar11 + 0x20);
          iVar12 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),1);
          iVar13 = (iVar13 + iVar8 * (uint)bVar1 + iVar3 + iVar9) * iVar15 + iVar4 + iVar5 + iVar11
                   + *(int *)(iVar12 + 0x20) * (uint)*(byte *)(param_1 + 0x9c);
          piVar10 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                     (*(undefined4 *)(param_1 + 0x118),4);
          (**(code **)(*piVar10 + 0x80))
                    (piVar10,iVar13,
                     *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84));
          piVar10 = local_5c;
          piVar17 = local_3c;
          for (iVar13 = 4; iVar13 != 0; iVar13 = iVar13 + -1) {
            *piVar17 = *piVar10;
            piVar10 = piVar10 + (uint)bVar18 * -2 + 1;
            piVar17 = piVar17 + (uint)bVar18 * -2 + 1;
          }
          iVar13 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                             (*(undefined4 *)(param_1 + 0x118),0,0,local_4c);
          iVar13 = *(int *)(iVar13 + 0x20);
          iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),1);
          iVar8 = *(int *)(iVar8 + 0x20);
          bVar1 = *(byte *)(param_1 + 0x9c);
          iVar3 = *(int *)(param_1 + 0xa0);
          iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),2);
          iVar9 = *(int *)(iVar9 + 0x20);
          iVar4 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
          iVar5 = *(int *)(param_1 + 0x80);
          iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),0);
          iVar11 = *(int *)(iVar11 + 0x20);
          iVar12 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),1);
          iVar12 = *(int *)(iVar12 + 0x20);
          bVar2 = *(byte *)(param_1 + 0x9c);
          iVar6 = *(int *)(param_1 + 0xa0);
          piVar10 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                     (*(undefined4 *)(param_1 + 0x118),5);
          (**(code **)(*piVar10 + 0x80))
                    (piVar10,(iVar13 + iVar8 * (uint)bVar1 + iVar3 + iVar9) * iVar15 + iVar4 + iVar5
                             + iVar11 + iVar12 * (uint)bVar2 + iVar6,
                     *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84));
          if (*(int *)(param_1 + 0x88) != 0) {
            ImageLib_TImageLibraryFont_SetColor_23EDC2EF
                      (*(int *)(param_1 + 0x88),*(undefined4 *)(param_1 + 0x24));
            (**(code **)(**(int **)(param_1 + 0x110) + 0xc))
                      (*(int **)(param_1 + 0x110),iVar15,&local_60);
            iVar13 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),0);
            iVar13 = *(int *)(iVar13 + 0x20);
            iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),1);
            iVar8 = *(int *)(iVar8 + 0x20);
            bVar1 = *(byte *)(param_1 + 0x9c);
            iVar3 = *(int *)(param_1 + 0xa0);
            iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),2);
            iVar9 = *(int *)(iVar9 + 0x20);
            iVar4 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
            iVar5 = *(int *)(param_1 + 0x80);
            iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),0);
            iVar11 = *(int *)(iVar11 + 0x20);
            iVar12 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),1);
            iVar12 = iVar11 + *(int *)(iVar12 + 0x20) * (uint)*(byte *)(param_1 + 0x9c) +
                     *(int *)(param_1 + 0xa0);
            iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),2);
            uVar14 = iVar12 + *(int *)(iVar11 + 0x20);
            iVar11 = (int)uVar14 >> 1;
            if (iVar11 < 0) {
              iVar11 = iVar11 + (uint)((uVar14 & 1) != 0);
            }
            iVar11 = (iVar13 + iVar8 * (uint)bVar1 + iVar3 + iVar9) * iVar15 + iVar4 + iVar5 +
                     iVar11;
            (**(code **)(**(int **)(param_1 + 0x110) + 0xc))
                      (*(int **)(param_1 + 0x110),iVar15,&local_64);
            uVar14 = ImageLib_TImageLibraryFont_GetTextWidth_23EDC2EF
                               (*(undefined4 *)(param_1 + 0x88),local_64);
            iVar13 = (int)uVar14 >> 1;
            if (iVar13 < 0) {
              iVar13 = iVar13 + (uint)((uVar14 & 1) != 0);
            }
            ImageLib_TImageLibraryFont_SetText_23EDC2EF
                      (*(undefined4 *)(param_1 + 0x88),iVar11 - iVar13,
                       *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84) +
                       *(int *)(param_1 + 0x98));
          }
        }
        iVar15 = iVar15 + 1;
        local_c = local_c + -1;
      } while (local_c != 0);
    }
    puVar7 = puStack_74;
    if (*(short *)(param_1 + 0x32) != 0) {
      (**(code **)(param_1 + 0x30))(*(undefined4 *)(param_1 + 0x34),param_1,local_4c);
      puVar7 = puStack_74;
    }
  }
  puStack_74 = puVar7;
  puVar7 = puStack_74;
  *in_FS_OFFSET = uStack_7c;
  puStack_74 = &LAB_00414d4e;
  puStack_78 = (undefined1 *)0x414d46;
  System__LStrArrayClr_51F89FF7(&local_64,2,puVar7);
  return;
}

