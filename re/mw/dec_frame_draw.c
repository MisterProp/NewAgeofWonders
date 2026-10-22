void AoWFrame_TAoWFrame_Draw_23EDC2EF(int param_1,undefined4 *param_2)
{
  int *piVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  uint uVar10;
  int iVar11;
  undefined4 *puVar12;
  undefined4 *puVar13;
  byte bVar14;
  int iStack_64;
  int iStack_60;
  undefined4 local_5c [4];
  int local_4c [9];
  undefined4 uStack_28;
  undefined4 uStack_24;
  int local_20 [4];
                    /* 0x1d91c  415  AoWFrame.TAoWFrame.Draw@23EDC2EF */
  bVar14 = 0;
  puVar12 = local_5c;
  for (iVar9 = 0xf; iVar9 != 0; iVar9 = iVar9 + -1) {
    *puVar12 = *param_2;
    param_2 = param_2 + 1;
    puVar12 = puVar12 + 1;
  }
  if ((*(char *)(param_1 + 0x2c) == '\x01') && (*(char *)(param_1 + 0x69) == '\x01')) {
    piVar1 = local_4c;
    piVar4 = local_20;
    for (iVar9 = 4; iVar9 != 0; iVar9 = iVar9 + -1) {
      *piVar4 = *piVar1;
      piVar1 = piVar1 + 1;
      piVar4 = piVar4 + 1;
    }
    if (*(char *)(param_1 + 0x110) != '\x03') {
      if (*(char *)(param_1 + 0x110) == '\x02') {
        piVar1 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(param_1 + 0x11c),0,0,local_5c);
        (**(code **)(*piVar1 + 0x80))
                  (piVar1,*(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80),
                   *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84));
        uVar10 = (uint)*(byte *)(param_1 + 0x9c);
        if (-1 < (int)(uVar10 - 1)) {
          iVar9 = 0;
          do {
            piVar1 = &iStack_60;
            iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x11c),0);
            iVar2 = *(int *)(iVar2 + 0x20);
            iVar6 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
            iVar5 = *(int *)(param_1 + 0x80);
            iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x11c),1);
            iVar2 = iVar2 + iVar6 + iVar5 + *(int *)(iVar3 + 0x20) * iVar9;
            piVar4 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                      (*(undefined4 *)(param_1 + 0x11c),1);
            (**(code **)(*piVar4 + 0x80))
                      (piVar4,iVar2,
                       *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84),piVar1)
            ;
            iVar9 = iVar9 + 1;
            uVar10 = uVar10 - 1;
          } while (uVar10 != 0);
        }
        iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x11c),2);
        if ((*(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80) +
            *(int *)(param_1 + 0x78)) - *(int *)(iVar9 + 0x20) < local_20[1]) {
          iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x11c),2);
          local_4c[1] = (*(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80) +
                        *(int *)(param_1 + 0x78)) - *(int *)(iVar9 + 0x20);
        }
        piVar1 = &iStack_60;
        iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x11c),0);
        iVar9 = *(int *)(iVar9 + 0x20);
        iVar2 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
        iVar6 = *(int *)(param_1 + 0x80);
        iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x11c),1);
        iVar9 = iVar9 + iVar2 + iVar6 + *(int *)(iVar5 + 0x20) * (uint)*(byte *)(param_1 + 0x9c);
        piVar4 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(param_1 + 0x11c),1);
        (**(code **)(*piVar4 + 0x80))
                  (piVar4,iVar9,
                   *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84),piVar1);
        puVar12 = &uStack_28;
        puVar13 = local_5c + 2;
        for (iVar9 = 4; iVar9 != 0; iVar9 = iVar9 + -1) {
          *puVar13 = *puVar12;
          puVar12 = puVar12 + (uint)bVar14 * -2 + 1;
          puVar13 = puVar13 + (uint)bVar14 * -2 + 1;
        }
        iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                          (*(undefined4 *)(param_1 + 0x11c),2,0,&iStack_64);
        iVar9 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
        iVar2 = *(int *)(param_1 + 0x80);
        iVar6 = *(int *)(param_1 + 0x78);
        iVar5 = *(int *)(iVar5 + 0x20);
        piVar1 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(param_1 + 0x11c),2);
        (**(code **)(*piVar1 + 0x80))
                  (piVar1,(iVar9 + iVar2 + iVar6) - iVar5,
                   *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84));
      }
      else {
        piVar1 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(param_1 + 0x114),0,0,local_5c);
        (**(code **)(*piVar1 + 0x80))
                  (piVar1,*(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80),
                   *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84));
        uVar10 = (uint)*(byte *)(param_1 + 0x9c);
        if (-1 < (int)(uVar10 - 1)) {
          iVar9 = 0;
          do {
            piVar1 = &iStack_60;
            iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),0);
            iVar2 = *(int *)(iVar2 + 0x20);
            iVar6 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
            iVar5 = *(int *)(param_1 + 0x80);
            iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),1);
            iVar2 = iVar2 + iVar6 + iVar5 + *(int *)(iVar3 + 0x20) * iVar9;
            piVar4 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                      (*(undefined4 *)(param_1 + 0x114),1);
            (**(code **)(*piVar4 + 0x80))
                      (piVar4,iVar2,
                       *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84),piVar1)
            ;
            iVar9 = iVar9 + 1;
            uVar10 = uVar10 - 1;
          } while (uVar10 != 0);
        }
        iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),2);
        if ((*(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80) +
            *(int *)(param_1 + 0x78)) - *(int *)(iVar9 + 0x20) < local_20[1]) {
          iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),2);
          local_4c[1] = (*(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80) +
                        *(int *)(param_1 + 0x78)) - *(int *)(iVar9 + 0x20);
        }
        piVar1 = &iStack_60;
        iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),0);
        iVar9 = *(int *)(iVar9 + 0x20);
        iVar2 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
        iVar6 = *(int *)(param_1 + 0x80);
        iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),1);
        iVar9 = iVar9 + iVar2 + iVar6 + *(int *)(iVar5 + 0x20) * (uint)*(byte *)(param_1 + 0x9c);
        piVar4 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(param_1 + 0x114),1);
        (**(code **)(*piVar4 + 0x80))
                  (piVar4,iVar9,
                   *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84),piVar1);
        puVar12 = &uStack_28;
        puVar13 = local_5c + 2;
        for (iVar9 = 4; iVar9 != 0; iVar9 = iVar9 + -1) {
          *puVar13 = *puVar12;
          puVar12 = puVar12 + (uint)bVar14 * -2 + 1;
          puVar13 = puVar13 + (uint)bVar14 * -2 + 1;
        }
        iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                          (*(undefined4 *)(param_1 + 0x114),2,0,&iStack_64);
        iVar9 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
        iVar2 = *(int *)(param_1 + 0x80);
        iVar6 = *(int *)(param_1 + 0x78);
        iVar5 = *(int *)(iVar5 + 0x20);
        piVar1 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(param_1 + 0x114),2);
        (**(code **)(*piVar1 + 0x80))
                  (piVar1,(iVar9 + iVar2 + iVar6) - iVar5,
                   *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84));
      }
    }
    if (*(char *)(param_1 + 0x110) != '\x02') {
      if (*(char *)(param_1 + 0x110) == '\x03') {
        puVar12 = local_5c;
        piVar1 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(param_1 + 0x11c),0);
        (**(code **)(*piVar1 + 0x80))
                  (piVar1,*(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80),
                   *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84),puVar12);
        uVar10 = (uint)*(byte *)(param_1 + 0x9d);
        if (-1 < (int)(uVar10 - 1)) {
          iVar9 = 0;
          do {
            piVar1 = &iStack_60;
            iVar6 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x11c),0);
            iVar2 = *(int *)(param_1 + 0x108);
            iVar6 = *(int *)(iVar6 + 0x24) + *(int *)(iVar2 + 0x84) + *(int *)(param_1 + 0x84);
            iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x11c),1);
            iVar6 = iVar6 + *(int *)(iVar5 + 0x24) * iVar9;
            piVar4 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                      (*(undefined4 *)(param_1 + 0x11c),1);
            (**(code **)(*piVar4 + 0x80))
                      (piVar4,*(int *)(iVar2 + 0x80) + *(int *)(param_1 + 0x80),iVar6,piVar1);
            iVar9 = iVar9 + 1;
            uVar10 = uVar10 - 1;
          } while (uVar10 != 0);
        }
        iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x11c),0);
        iVar9 = *(int *)(iVar9 + 0x24);
        iVar2 = *(int *)(*(int *)(param_1 + 0x108) + 0x84);
        iVar6 = *(int *)(param_1 + 0x84);
        iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x11c),1);
        if ((int)(iVar9 + iVar2 + iVar6 + *(int *)(iVar5 + 0x24) * (uint)*(byte *)(param_1 + 0x9d) +
                 *(int *)(param_1 + 0xa4)) < local_20[2]) {
          iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x11c),0);
          iVar9 = *(int *)(iVar9 + 0x24);
          iVar2 = *(int *)(*(int *)(param_1 + 0x108) + 0x84);
          iVar6 = *(int *)(param_1 + 0x84);
          iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x11c),1);
          local_4c[2] = iVar9 + iVar2 + iVar6 +
                        *(int *)(iVar5 + 0x24) * (uint)*(byte *)(param_1 + 0x9d) +
                        *(int *)(param_1 + 0xa4);
        }
        piVar1 = &iStack_60;
        iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x11c),0);
        iVar9 = *(int *)(iVar9 + 0x24);
        iVar2 = *(int *)(param_1 + 0x108);
        iVar6 = *(int *)(iVar2 + 0x84);
        iVar5 = *(int *)(param_1 + 0x84);
        iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x11c),1);
        iVar9 = iVar9 + iVar6 + iVar5 + *(int *)(iVar3 + 0x24) * (uint)*(byte *)(param_1 + 0x9d);
        piVar4 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(param_1 + 0x11c),1);
        (**(code **)(*piVar4 + 0x80))
                  (piVar4,*(int *)(iVar2 + 0x80) + *(int *)(param_1 + 0x80),iVar9,piVar1);
        puVar12 = &uStack_28;
        puVar13 = local_5c + 2;
        for (iVar9 = 4; iVar9 != 0; iVar9 = iVar9 + -1) {
          *puVar13 = *puVar12;
          puVar12 = puVar12 + (uint)bVar14 * -2 + 1;
          puVar13 = puVar13 + (uint)bVar14 * -2 + 1;
        }
        iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                          (*(undefined4 *)(param_1 + 0x11c),2,0,&iStack_64);
        iVar9 = *(int *)(param_1 + 0x108);
        iVar2 = *(int *)(iVar9 + 0x84);
        iVar6 = *(int *)(param_1 + 0x84);
        iVar5 = *(int *)(param_1 + 0x7c);
        iVar3 = *(int *)(iVar3 + 0x24);
        piVar1 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(param_1 + 0x11c),2);
        (**(code **)(*piVar1 + 0x80))
                  (piVar1,*(int *)(iVar9 + 0x80) + *(int *)(param_1 + 0x80),
                   (iVar2 + iVar6 + iVar5) - iVar3);
      }
      else {
        uVar10 = (uint)*(byte *)(param_1 + 0x9d);
        if (-1 < (int)(uVar10 - 1)) {
          iVar9 = 0;
          do {
            puVar12 = local_5c;
            iVar6 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),0);
            iVar2 = *(int *)(param_1 + 0x108);
            iVar6 = *(int *)(iVar6 + 0x24) + *(int *)(iVar2 + 0x84) + *(int *)(param_1 + 0x84);
            iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),3);
            iVar6 = iVar6 + *(int *)(iVar5 + 0x24) * iVar9;
            piVar1 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                      (*(undefined4 *)(param_1 + 0x114),3);
            (**(code **)(*piVar1 + 0x80))
                      (piVar1,*(int *)(iVar2 + 0x80) + *(int *)(param_1 + 0x80),iVar6,puVar12);
            iVar9 = iVar9 + 1;
            uVar10 = uVar10 - 1;
          } while (uVar10 != 0);
        }
        iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),0);
        iVar9 = *(int *)(iVar9 + 0x24);
        iVar2 = *(int *)(*(int *)(param_1 + 0x108) + 0x84);
        iVar6 = *(int *)(param_1 + 0x84);
        iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),3);
        if ((int)(iVar9 + iVar2 + iVar6 + *(int *)(iVar5 + 0x24) * (uint)*(byte *)(param_1 + 0x9d) +
                 *(int *)(param_1 + 0xa4)) < local_20[3]) {
          iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),0);
          iVar9 = *(int *)(iVar9 + 0x24);
          iVar2 = *(int *)(*(int *)(param_1 + 0x108) + 0x84);
          iVar6 = *(int *)(param_1 + 0x84);
          iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),3);
          local_4c[3] = iVar9 + iVar2 + iVar6 +
                        *(int *)(iVar5 + 0x24) * (uint)*(byte *)(param_1 + 0x9d) +
                        *(int *)(param_1 + 0xa4);
        }
        puVar12 = local_5c;
        iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),0);
        iVar9 = *(int *)(iVar9 + 0x24);
        iVar2 = *(int *)(param_1 + 0x108);
        iVar6 = *(int *)(iVar2 + 0x84);
        iVar5 = *(int *)(param_1 + 0x84);
        iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),3);
        iVar9 = iVar9 + iVar6 + iVar5 + *(int *)(iVar3 + 0x24) * (uint)*(byte *)(param_1 + 0x9d);
        piVar1 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(param_1 + 0x114),3);
        (**(code **)(*piVar1 + 0x80))
                  (piVar1,*(int *)(iVar2 + 0x80) + *(int *)(param_1 + 0x80),iVar9,puVar12);
        puVar12 = &uStack_24;
        puVar13 = local_5c + 3;
        for (iVar9 = 4; iVar9 != 0; iVar9 = iVar9 + -1) {
          *puVar13 = *puVar12;
          puVar12 = puVar12 + (uint)bVar14 * -2 + 1;
          puVar13 = puVar13 + (uint)bVar14 * -2 + 1;
        }
        iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                          (*(undefined4 *)(param_1 + 0x114),5,0,&iStack_60);
        iVar9 = *(int *)(param_1 + 0x108);
        iVar2 = (*(int *)(iVar9 + 0x84) + *(int *)(param_1 + 0x84) + *(int *)(param_1 + 0x7c)) -
                *(int *)(iVar2 + 0x24);
        piVar1 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(param_1 + 0x114),5);
        (**(code **)(*piVar1 + 0x80))
                  (piVar1,*(int *)(iVar9 + 0x80) + *(int *)(param_1 + 0x80),iVar2);
      }
    }
    if (*(char *)(param_1 + 0x110) == '\0') {
      iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),2);
      if ((*(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80) +
          *(int *)(param_1 + 0x78)) - *(int *)(iVar9 + 0x20) < local_20[2]) {
        iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),2);
        local_4c[2] = (*(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80) +
                      *(int *)(param_1 + 0x78)) - *(int *)(iVar9 + 0x20);
      }
      iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),5);
      if ((*(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84) +
          *(int *)(param_1 + 0x7c)) - *(int *)(iVar9 + 0x24) < local_20[3]) {
        iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),5);
        local_4c[3] = (*(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84) +
                      *(int *)(param_1 + 0x7c)) - *(int *)(iVar9 + 0x24);
      }
      iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),5);
      iVar2 = *(int *)(param_1 + 0x7c) - *(int *)(iVar9 + 0x24);
      iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),0);
      iVar2 = iVar2 / *(int *)(iVar9 + 0x24);
      if (-1 < iVar2) {
        iVar2 = iVar2 + 1;
        iStack_64 = 0;
        do {
          iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),2);
          iVar6 = *(int *)(param_1 + 0x78) - *(int *)(iVar9 + 0x20);
          iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),0);
          iVar6 = iVar6 / *(int *)(iVar9 + 0x20);
          if (-1 < iVar6) {
            iStack_60 = iVar6 + 1;
            iVar9 = 0;
            do {
              iVar6 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),0);
              iVar6 = *(int *)(iVar6 + 0x24);
              iVar5 = *(int *)(*(int *)(param_1 + 0x108) + 0x84);
              iVar3 = *(int *)(param_1 + 0x84);
              iVar7 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),0);
              iVar11 = iVar6 + iVar5 + iVar3 + *(int *)(iVar7 + 0x24) * iStack_64;
              iVar6 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),3);
              iVar6 = *(int *)(iVar6 + 0x20);
              iVar5 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
              iVar3 = *(int *)(param_1 + 0x80);
              iVar7 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x118),0);
              iVar6 = iVar6 + iVar5 + iVar3 + *(int *)(iVar7 + 0x20) * iVar9;
              piVar1 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                        (*(undefined4 *)(param_1 + 0x118),0);
              (**(code **)(*piVar1 + 0x80))(piVar1,iVar6,iVar11);
              iVar9 = iVar9 + 1;
              iStack_60 = iStack_60 + -1;
            } while (iStack_60 != 0);
          }
          iStack_64 = iStack_64 + 1;
          iVar2 = iVar2 + -1;
        } while (iVar2 != 0);
      }
      piVar1 = local_20;
      piVar4 = local_4c;
      for (iVar9 = 4; iVar9 != 0; iVar9 = iVar9 + -1) {
        *piVar4 = *piVar1;
        piVar1 = piVar1 + (uint)bVar14 * -2 + 1;
        piVar4 = piVar4 + (uint)bVar14 * -2 + 1;
      }
    }
    if ((*(char *)(param_1 + 0x110) == '\0') || (*(char *)(param_1 + 0x110) == '\x01')) {
      uVar10 = (uint)*(byte *)(param_1 + 0x9d);
      if (-1 < (int)(uVar10 - 1)) {
        iVar9 = 0;
        do {
          puVar12 = local_5c;
          iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),0);
          iVar2 = *(int *)(iVar2 + 0x24);
          iVar6 = *(int *)(*(int *)(param_1 + 0x108) + 0x84);
          iVar5 = *(int *)(param_1 + 0x84);
          iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),3);
          iVar6 = iVar2 + iVar6 + iVar5 + *(int *)(iVar3 + 0x24) * iVar9;
          iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),4);
          iVar2 = (*(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80) +
                  *(int *)(param_1 + 0x78)) - *(int *)(iVar2 + 0x20);
          piVar1 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                    (*(undefined4 *)(param_1 + 0x114),4);
          (**(code **)(*piVar1 + 0x80))(piVar1,iVar2,iVar6,puVar12);
          iVar9 = iVar9 + 1;
          uVar10 = uVar10 - 1;
        } while (uVar10 != 0);
      }
      iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),0);
      iVar9 = *(int *)(iVar9 + 0x24);
      iVar2 = *(int *)(*(int *)(param_1 + 0x108) + 0x84);
      iVar6 = *(int *)(param_1 + 0x84);
      iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),3);
      if ((int)(iVar9 + iVar2 + iVar6 + *(int *)(iVar5 + 0x24) * (uint)*(byte *)(param_1 + 0x9d) +
               *(int *)(param_1 + 0xa4)) < local_20[3]) {
        iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),0);
        iVar9 = *(int *)(iVar9 + 0x24);
        iVar2 = *(int *)(*(int *)(param_1 + 0x108) + 0x84);
        iVar6 = *(int *)(param_1 + 0x84);
        iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),3);
        local_4c[3] = iVar9 + iVar2 + iVar6 +
                      *(int *)(iVar5 + 0x24) * (uint)*(byte *)(param_1 + 0x9d) +
                      *(int *)(param_1 + 0xa4);
      }
      puVar12 = local_5c;
      iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),0);
      iVar9 = *(int *)(iVar9 + 0x24);
      iVar2 = *(int *)(*(int *)(param_1 + 0x108) + 0x84);
      iVar6 = *(int *)(param_1 + 0x84);
      iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),3);
      iVar2 = iVar9 + iVar2 + iVar6 + *(int *)(iVar5 + 0x24) * (uint)*(byte *)(param_1 + 0x9d);
      iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),4);
      iVar9 = (*(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80) +
              *(int *)(param_1 + 0x78)) - *(int *)(iVar9 + 0x20);
      piVar1 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),4);
      (**(code **)(*piVar1 + 0x80))(piVar1,iVar9,iVar2,puVar12);
      puVar12 = &uStack_24;
      puVar13 = local_5c + 3;
      for (iVar9 = 4; iVar9 != 0; iVar9 = iVar9 + -1) {
        *puVar13 = *puVar12;
        puVar12 = puVar12 + (uint)bVar14 * -2 + 1;
        puVar13 = puVar13 + (uint)bVar14 * -2 + 1;
      }
      uVar10 = (uint)*(byte *)(param_1 + 0x9c);
      if (-1 < (int)(uVar10 - 1)) {
        iVar9 = 0;
        do {
          piVar1 = &iStack_60;
          iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),5);
          iVar2 = *(int *)(iVar2 + 0x20);
          iVar6 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
          iVar5 = *(int *)(param_1 + 0x80);
          iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),6);
          iVar7 = iVar2 + iVar6 + iVar5 + *(int *)(iVar3 + 0x20) * iVar9;
          iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),5);
          iVar2 = *(int *)(*(int *)(param_1 + 0x108) + 0x84);
          iVar6 = *(int *)(param_1 + 0x84);
          iVar5 = *(int *)(param_1 + 0x7c);
          iVar3 = *(int *)(iVar3 + 0x24);
          piVar4 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                    (*(undefined4 *)(param_1 + 0x114),6);
          (**(code **)(*piVar4 + 0x80))(piVar4,iVar7,(iVar2 + iVar6 + iVar5) - iVar3,piVar1);
          iVar9 = iVar9 + 1;
          uVar10 = uVar10 - 1;
        } while (uVar10 != 0);
      }
      iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),2);
      if ((*(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80) +
          *(int *)(param_1 + 0x78)) - *(int *)(iVar9 + 0x20) < local_20[1]) {
        iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),2);
        local_4c[1] = (*(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80) +
                      *(int *)(param_1 + 0x78)) - *(int *)(iVar9 + 0x20);
      }
      piVar1 = &iStack_60;
      iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),0);
      iVar9 = *(int *)(iVar9 + 0x20);
      iVar2 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
      iVar6 = *(int *)(param_1 + 0x80);
      iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),6);
      iVar3 = iVar9 + iVar2 + iVar6 + *(int *)(iVar5 + 0x20) * (uint)*(byte *)(param_1 + 0x9c);
      iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),5);
      iVar9 = *(int *)(*(int *)(param_1 + 0x108) + 0x84);
      iVar2 = *(int *)(param_1 + 0x84);
      iVar6 = *(int *)(param_1 + 0x7c);
      iVar5 = *(int *)(iVar5 + 0x24);
      piVar4 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),6);
      (**(code **)(*piVar4 + 0x80))(piVar4,iVar3,(iVar9 + iVar2 + iVar6) - iVar5,piVar1);
      puVar12 = &uStack_28;
      puVar13 = local_5c + 2;
      for (iVar9 = 4; iVar9 != 0; iVar9 = iVar9 + -1) {
        *puVar13 = *puVar12;
        puVar12 = puVar12 + (uint)bVar14 * -2 + 1;
        puVar13 = puVar13 + (uint)bVar14 * -2 + 1;
      }
      iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                        (*(undefined4 *)(param_1 + 0x114),7,0,&iStack_64);
      iVar9 = *(int *)(*(int *)(param_1 + 0x108) + 0x84);
      iVar2 = *(int *)(param_1 + 0x84);
      iVar6 = *(int *)(param_1 + 0x7c);
      iVar5 = *(int *)(iVar5 + 0x24);
      iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),7);
      iVar3 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
      iVar7 = *(int *)(param_1 + 0x80);
      iVar11 = *(int *)(param_1 + 0x78);
      iVar8 = *(int *)(iVar8 + 0x20);
      piVar1 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x114),7);
      (**(code **)(*piVar1 + 0x80))
                (piVar1,(iVar3 + iVar7 + iVar11) - iVar8,(iVar9 + iVar2 + iVar6) - iVar5);
    }
    if (*(short *)(param_1 + 0x32) != 0) {
      (**(code **)(param_1 + 0x30))(*(undefined4 *)(param_1 + 0x34),param_1,local_5c);
    }
  }
  return;
}
