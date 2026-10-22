
void AoWWindow_TAOWWindow_Draw_23EDC2EF(int param_1,int *param_2)

{
  char cVar1;
  byte bVar2;
  int iVar3;
  uint *puVar4;
  int iVar5;
  int *piVar6;
  int *piVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  byte bVar13;
  uint *puStack_6c;
  int *piStack_68;
  int iStack_64;
  uint local_60;
  int local_5c [4];
  int local_4c [7];
  int aiStack_30 [2];
  int iStack_28;
  int iStack_24;
  int local_20 [4];
  
                    /* 0x7654  136  AoWWindow.TAOWWindow.Draw@23EDC2EF */
  bVar13 = 0;
  piVar6 = local_5c;
  for (iVar11 = 0xf; iVar11 != 0; iVar11 = iVar11 + -1) {
    *piVar6 = *param_2;
    param_2 = param_2 + 1;
    piVar6 = piVar6 + 1;
  }
  if ((*(char *)(param_1 + 0x69) == '\x01') && (*(char *)(param_1 + 0xd2) == '\0')) {
    piVar6 = local_4c;
    piVar7 = local_20;
    for (iVar11 = 4; iVar11 != 0; iVar11 = iVar11 + -1) {
      *piVar7 = *piVar6;
      piVar6 = piVar6 + 1;
      piVar7 = piVar7 + 1;
    }
    if (((*(char *)(param_1 + 0x149) == '\0') || (*(char *)(param_1 + 0x149) == '\x01')) &&
       (*(int *)(param_1 + 0x1b8) != 0)) {
      if (*(char *)(param_1 + 0x128) == '\x01') {
        iVar11 = *(int *)(param_1 + 0x80);
      }
      else {
        piStack_68 = (int *)0x4076cc;
        iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(int *)(param_1 + 0x1b8),0);
        iVar11 = *(int *)(iVar11 + 0x20) + *(int *)(param_1 + 0x80);
      }
      if (-1 < (int)(*(byte *)(param_1 + 0x9c) - 1)) {
        iVar12 = 0;
        local_60 = (uint)*(byte *)(param_1 + 0x9c);
        do {
          piStack_68 = local_5c;
          puStack_6c = (uint *)0x407700;
          iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),1);
          puStack_6c = (uint *)(*(int *)(iVar5 + 0x20) * iVar12 + iVar11);
          piVar6 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                    (*(undefined4 *)(param_1 + 0x1b8),1);
          puVar4 = puStack_6c;
          puStack_6c = (uint *)0x407727;
          (**(code **)(*piVar6 + 0x80))(piVar6,puVar4,*(undefined4 *)(param_1 + 0x84));
          iVar12 = iVar12 + 1;
          local_60 = local_60 - 1;
        } while (local_60 != 0);
      }
      piStack_68 = (int *)0x40773e;
      iVar12 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),1);
      if ((int)(*(int *)(iVar12 + 0x20) * (uint)*(byte *)(param_1 + 0x9c) + iVar11 +
               *(int *)(param_1 + 0xa0)) < local_20[2]) {
        piStack_68 = (int *)0x407769;
        iVar12 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),1);
        local_4c[2] = *(int *)(iVar12 + 0x20) * (uint)*(byte *)(param_1 + 0x9c) + iVar11 +
                      *(int *)(param_1 + 0xa0);
      }
      piStack_68 = local_5c;
      puStack_6c = (uint *)0x407797;
      iVar12 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),1);
      puStack_6c = (uint *)(iVar11 + *(int *)(iVar12 + 0x20) * (uint)*(byte *)(param_1 + 0x9c));
      piVar6 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),1);
      puVar4 = puStack_6c;
      puStack_6c = (uint *)0x4077c6;
      (**(code **)(*piVar6 + 0x80))(piVar6,puVar4,*(undefined4 *)(param_1 + 0x84));
      piVar6 = &iStack_24;
      piVar7 = local_5c + 3;
      for (iVar11 = 4; iVar11 != 0; iVar11 = iVar11 + -1) {
        *piVar7 = *piVar6;
        piVar6 = piVar6 + (uint)bVar13 * -2 + 1;
        piVar7 = piVar7 + (uint)bVar13 * -2 + 1;
      }
      puStack_6c = &local_60;
      piVar6 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),0);
      (**(code **)(*piVar6 + 0x80))
                (piVar6,*(undefined4 *)(param_1 + 0x80),*(undefined4 *)(param_1 + 0x84));
      piVar6 = &iStack_64;
      iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),2);
      iVar11 = *(int *)(param_1 + 0x80);
      iVar12 = *(int *)(param_1 + 0x78);
      iVar5 = *(int *)(iVar5 + 0x20);
      piVar7 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),2);
      (**(code **)(*piVar7 + 0x80))
                (piVar7,(iVar11 + iVar12) - iVar5,*(undefined4 *)(param_1 + 0x84),piVar6);
    }
    cVar1 = *(char *)(param_1 + 0x149);
    if ((cVar1 == '\0') || (cVar1 == '\x02')) {
      if (cVar1 == '\0') {
        if ((*(int *)(param_1 + 0x1b8) != 0) && (*(int *)(param_1 + 0x1bc) != 0)) {
          piStack_68 = (int *)0x40787f;
          iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(int *)(param_1 + 0x1b8),3);
          if ((*(int *)(param_1 + 0x80) + *(int *)(param_1 + 0x78)) - *(int *)(iVar11 + 0x20) <
              local_20[2]) {
            piStack_68 = (int *)0x4078a1;
            iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),3);
            local_4c[2] = (*(int *)(param_1 + 0x80) + *(int *)(param_1 + 0x78)) -
                          *(int *)(iVar11 + 0x20);
          }
          piStack_68 = (int *)0x4078be;
          iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),0);
          iVar11 = *(int *)(iVar11 + 0x24);
          iVar12 = *(int *)(param_1 + 0x84);
          piStack_68 = (int *)0x4078d7;
          iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),3);
          if ((int)(iVar11 + iVar12 + *(int *)(iVar5 + 0x24) * (uint)*(byte *)(param_1 + 0x9d) +
                   *(int *)(param_1 + 0xa4)) < local_20[3]) {
            piStack_68 = (int *)0x4078ff;
            iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),0);
            iVar11 = *(int *)(iVar11 + 0x24);
            iVar12 = *(int *)(param_1 + 0x84);
            piStack_68 = (int *)0x407918;
            iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),3);
            local_4c[3] = iVar11 + iVar12 + *(int *)(iVar5 + 0x24) * (uint)*(byte *)(param_1 + 0x9d)
                          + *(int *)(param_1 + 0xa4);
          }
          piStack_68 = (int *)0x407941;
          iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),3);
          piStack_68 = (int *)(*(int *)(iVar11 + 0x24) * (uint)*(byte *)(param_1 + 0x9d) +
                              *(int *)(param_1 + 0xa4));
          puStack_6c = (uint *)0x407962;
          iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1bc),0);
          iVar11 = (int)piStack_68 / *(int *)(iVar11 + 0x24);
          if (-1 < iVar11) {
            local_60 = iVar11 + 1;
            iStack_64 = 0;
            do {
              piStack_68 = (int *)0x407990;
              iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),3)
              ;
              piStack_68 = (int *)(*(int *)(param_1 + 0x78) + *(int *)(iVar11 + 0x20) * -2);
              puStack_6c = (uint *)0x4079a8;
              iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1bc),0)
              ;
              iVar11 = (int)piStack_68 / *(int *)(iVar11 + 0x20);
              if (-1 < iVar11) {
                iVar11 = iVar11 + 1;
                iVar12 = 0;
                do {
                  piStack_68 = local_5c;
                  puStack_6c = (uint *)0x4079cd;
                  iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                    (*(undefined4 *)(param_1 + 0x1b8),0);
                  iVar5 = *(int *)(iVar5 + 0x24);
                  iVar9 = *(int *)(param_1 + 0x84);
                  puStack_6c = (uint *)0x4079e3;
                  iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                    (*(undefined4 *)(param_1 + 0x1bc),0);
                  puStack_6c = (uint *)(iVar5 + iVar9 + *(int *)(iVar8 + 0x24) * iStack_64);
                  iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                    (*(undefined4 *)(param_1 + 0x1b8),3);
                  iVar5 = *(int *)(iVar5 + 0x20);
                  iVar9 = *(int *)(param_1 + 0x80);
                  iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                    (*(undefined4 *)(param_1 + 0x1bc),0);
                  iVar5 = iVar5 + iVar9 + *(int *)(iVar8 + 0x20) * iVar12;
                  piVar6 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                            (*(undefined4 *)(param_1 + 0x1bc),0);
                  puVar4 = puStack_6c;
                  puStack_6c = (uint *)0x407a32;
                  (**(code **)(*piVar6 + 0x80))(piVar6,iVar5,puVar4);
                  iVar12 = iVar12 + 1;
                  iVar11 = iVar11 + -1;
                } while (iVar11 != 0);
              }
              iStack_64 = iStack_64 + 1;
              local_60 = local_60 - 1;
            } while (local_60 != 0);
            local_60 = 0;
          }
        }
      }
      else if (*(int *)(param_1 + 0x1bc) != 0) {
        iVar11 = *(int *)(param_1 + 0x80) + *(int *)(param_1 + 0x78);
        if (iVar11 < local_20[2]) {
          local_4c[2] = iVar11;
        }
        iVar11 = *(int *)(param_1 + 0x84) + *(int *)(param_1 + 0x7c);
        if (iVar11 < local_20[3]) {
          local_4c[3] = iVar11;
        }
        piStack_68 = (int *)0x407a85;
        iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(int *)(param_1 + 0x1bc),0);
        iVar11 = *(int *)(param_1 + 0x7c) / *(int *)(iVar11 + 0x24);
        if (-1 < iVar11) {
          local_60 = iVar11 + 1;
          iStack_64 = 0;
          do {
            piStack_68 = (int *)0x407ab2;
            iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1bc),0);
            iVar11 = *(int *)(param_1 + 0x78) / *(int *)(iVar11 + 0x20);
            if (-1 < iVar11) {
              iVar11 = iVar11 + 1;
              iVar12 = 0;
              do {
                piStack_68 = local_5c;
                puStack_6c = (uint *)0x407ad9;
                iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(param_1 + 0x1bc),0);
                puStack_6c = (uint *)(*(int *)(iVar5 + 0x24) * iStack_64 + *(int *)(param_1 + 0x84))
                ;
                iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(param_1 + 0x1bc),0);
                iVar5 = *(int *)(iVar5 + 0x20) * iVar12 + *(int *)(param_1 + 0x80);
                piVar6 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                          (*(undefined4 *)(param_1 + 0x1bc),0);
                puVar4 = puStack_6c;
                puStack_6c = (uint *)0x407b17;
                (**(code **)(*piVar6 + 0x80))(piVar6,iVar5,puVar4);
                iVar12 = iVar12 + 1;
                iVar11 = iVar11 + -1;
              } while (iVar11 != 0);
            }
            iStack_64 = iStack_64 + 1;
            local_60 = local_60 - 1;
          } while (local_60 != 0);
        }
      }
    }
    piVar6 = local_20;
    piVar7 = local_4c;
    for (iVar11 = 4; iVar11 != 0; iVar11 = iVar11 + -1) {
      *piVar7 = *piVar6;
      piVar6 = piVar6 + (uint)bVar13 * -2 + 1;
      piVar7 = piVar7 + (uint)bVar13 * -2 + 1;
    }
    if (((*(char *)(param_1 + 0x149) == '\0') || (*(char *)(param_1 + 0x149) == '\x01')) &&
       (*(int *)(param_1 + 0x1b8) != 0)) {
      if (-1 < (int)(*(byte *)(param_1 + 0x9d) - 1)) {
        iVar11 = 0;
        local_60 = (uint)*(byte *)(param_1 + 0x9d);
        do {
          piStack_68 = local_5c;
          puStack_6c = (uint *)0x407b7e;
          iVar12 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),0);
          iVar12 = *(int *)(iVar12 + 0x24);
          iVar5 = *(int *)(param_1 + 0x84);
          puStack_6c = (uint *)0x407b97;
          iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),3);
          puStack_6c = (uint *)(iVar12 + iVar5 + *(int *)(iVar9 + 0x24) * iVar11);
          piVar6 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                    (*(undefined4 *)(param_1 + 0x1b8),3);
          puVar4 = puStack_6c;
          puStack_6c = (uint *)0x407bbe;
          (**(code **)(*piVar6 + 0x80))(piVar6,*(undefined4 *)(param_1 + 0x80),puVar4);
          puStack_6c = &local_60;
          iVar12 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),0);
          iVar12 = *(int *)(iVar12 + 0x24);
          iVar5 = *(int *)(param_1 + 0x84);
          iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),3);
          iVar8 = iVar12 + iVar5 + *(int *)(iVar9 + 0x24) * iVar11;
          iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),4);
          iVar12 = *(int *)(param_1 + 0x80);
          iVar5 = *(int *)(param_1 + 0x78);
          iVar9 = *(int *)(iVar9 + 0x20);
          piVar6 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                    (*(undefined4 *)(param_1 + 0x1b8),4);
          (**(code **)(*piVar6 + 0x80))(piVar6,(iVar12 + iVar5) - iVar9,iVar8);
          iVar11 = iVar11 + 1;
          local_60 = local_60 - 1;
        } while (local_60 != 0);
      }
      piStack_68 = (int *)0x407c40;
      iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),0);
      iVar11 = *(int *)(iVar11 + 0x24);
      iVar12 = *(int *)(param_1 + 0x84);
      piStack_68 = (int *)0x407c59;
      iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),3);
      if ((int)(iVar11 + iVar12 + *(int *)(iVar5 + 0x24) * (uint)*(byte *)(param_1 + 0x9d) +
               *(int *)(param_1 + 0xa4)) < local_20[3]) {
        piStack_68 = (int *)0x407c81;
        iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),0);
        iVar11 = *(int *)(iVar11 + 0x24);
        iVar12 = *(int *)(param_1 + 0x84);
        piStack_68 = (int *)0x407c9a;
        iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),3);
        local_4c[3] = iVar11 + iVar12 + *(int *)(iVar5 + 0x24) * (uint)*(byte *)(param_1 + 0x9d) +
                      *(int *)(param_1 + 0xa4);
      }
      piStack_68 = local_5c;
      puStack_6c = (uint *)0x407cc5;
      iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),0);
      iVar11 = *(int *)(iVar11 + 0x24);
      iVar12 = *(int *)(param_1 + 0x84);
      puStack_6c = (uint *)0x407cde;
      iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),3);
      puStack_6c = (uint *)(iVar11 + iVar12 +
                           *(int *)(iVar5 + 0x24) * (uint)*(byte *)(param_1 + 0x9d));
      piVar6 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),3);
      puVar4 = puStack_6c;
      puStack_6c = (uint *)0x407d0d;
      (**(code **)(*piVar6 + 0x80))(piVar6,*(undefined4 *)(param_1 + 0x80),puVar4);
      puStack_6c = &local_60;
      iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),0);
      iVar11 = *(int *)(iVar11 + 0x24);
      iVar12 = *(int *)(param_1 + 0x84);
      iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),3);
      iVar9 = iVar11 + iVar12 + *(int *)(iVar5 + 0x24) * (uint)*(byte *)(param_1 + 0x9d);
      iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),4);
      iVar11 = *(int *)(param_1 + 0x80);
      iVar12 = *(int *)(param_1 + 0x78);
      iVar5 = *(int *)(iVar5 + 0x20);
      piVar6 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),4);
      (**(code **)(*piVar6 + 0x80))(piVar6,(iVar11 + iVar12) - iVar5,iVar9);
      piVar6 = &iStack_28;
      piVar7 = local_5c + 2;
      for (iVar11 = 4; iVar11 != 0; iVar11 = iVar11 + -1) {
        *piVar7 = *piVar6;
        piVar6 = piVar6 + (uint)bVar13 * -2 + 1;
        piVar7 = piVar7 + (uint)bVar13 * -2 + 1;
      }
      iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                         (*(undefined4 *)(param_1 + 0x1b8),0,0,&iStack_64);
      iVar11 = *(int *)(iVar11 + 0x24);
      iVar12 = *(int *)(param_1 + 0x84);
      iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),3);
      iVar5 = *(int *)(iVar5 + 0x24);
      bVar2 = *(byte *)(param_1 + 0x9d);
      iVar9 = *(int *)(param_1 + 0xa4);
      piVar6 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),5);
      (**(code **)(*piVar6 + 0x80))
                (piVar6,*(undefined4 *)(param_1 + 0x80),
                 iVar11 + iVar12 + iVar5 * (uint)bVar2 + iVar9);
      iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),7);
      if ((*(int *)(param_1 + 0x80) + *(int *)(param_1 + 0x78)) - *(int *)(iVar11 + 0x20) <
          iStack_24) {
        iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),7);
        local_5c[3] = (*(int *)(param_1 + 0x80) + *(int *)(param_1 + 0x78)) -
                      *(int *)(iVar11 + 0x20);
      }
      if (-1 < (int)((int)(uint)*(byte *)(param_1 + 0x9c) - 1U)) {
        iVar11 = 0;
        puStack_6c = (uint *)(uint)*(byte *)(param_1 + 0x9c);
        do {
          iVar12 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),1);
          iVar12 = *(int *)(iVar12 + 0x24);
          iVar5 = *(int *)(param_1 + 0x84);
          iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),3);
          iVar8 = iVar12 + iVar5 + *(int *)(iVar9 + 0x24) * (uint)*(byte *)(param_1 + 0x9d) +
                  *(int *)(param_1 + 0xa4);
          iVar12 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),5);
          iVar12 = *(int *)(iVar12 + 0x20);
          iVar5 = *(int *)(param_1 + 0x80);
          iVar9 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),6);
          iVar9 = *(int *)(iVar9 + 0x20);
          piVar6 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                    (*(undefined4 *)(param_1 + 0x1b8),6);
          (**(code **)(*piVar6 + 0x80))(piVar6,iVar12 + iVar5 + iVar9 * iVar11,iVar8);
          iVar11 = iVar11 + 1;
          puStack_6c = (uint *)((int)puStack_6c - 1);
        } while (puStack_6c != (uint *)0x0);
      }
      iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),1);
      iVar11 = *(int *)(iVar11 + 0x24);
      iVar12 = *(int *)(param_1 + 0x84);
      iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),3);
      iVar9 = iVar11 + iVar12 + *(int *)(iVar5 + 0x24) * (uint)*(byte *)(param_1 + 0x9d) +
              *(int *)(param_1 + 0xa4);
      iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),5);
      iVar11 = *(int *)(iVar11 + 0x20);
      iVar12 = *(int *)(param_1 + 0x80);
      iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),6);
      iVar5 = *(int *)(iVar5 + 0x20);
      bVar2 = *(byte *)(param_1 + 0x9c);
      piVar6 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),6);
      (**(code **)(*piVar6 + 0x80))(piVar6,iVar11 + iVar12 + iVar5 * (uint)bVar2,iVar9);
      piVar6 = aiStack_30;
      piVar7 = local_5c;
      for (iVar11 = 4; iVar11 != 0; iVar11 = iVar11 + -1) {
        *piVar7 = *piVar6;
        piVar6 = piVar6 + (uint)bVar13 * -2 + 1;
        piVar7 = piVar7 + (uint)bVar13 * -2 + 1;
      }
      iVar11 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                         (*(undefined4 *)(param_1 + 0x1b8),1,0,&puStack_6c);
      iVar11 = *(int *)(iVar11 + 0x24);
      iVar12 = *(int *)(param_1 + 0x84);
      iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),3);
      iVar5 = *(int *)(iVar5 + 0x24);
      bVar13 = *(byte *)(param_1 + 0x9d);
      iVar9 = *(int *)(param_1 + 0xa4);
      iVar10 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),7);
      iVar8 = *(int *)(param_1 + 0x80);
      iVar3 = *(int *)(param_1 + 0x78);
      iVar10 = *(int *)(iVar10 + 0x20);
      piVar6 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1b8),7);
      (**(code **)(*piVar6 + 0x80))
                (piVar6,(iVar8 + iVar3) - iVar10,iVar11 + iVar12 + iVar5 * (uint)bVar13 + iVar9);
    }
    if (*(short *)(param_1 + 0x19a) != 0) {
      piStack_68 = (int *)0x408023;
      (**(code **)(param_1 + 0x198))(*(undefined4 *)(param_1 + 0x19c),param_1,local_5c);
    }
    *(undefined1 *)(param_1 + 0x18c) = 0;
  }
  return;
}

