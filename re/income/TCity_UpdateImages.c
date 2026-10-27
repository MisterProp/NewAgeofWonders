

void City_TCity_UpdateImages_23EDC2EF(int *param_1)



{

  undefined4 uVar1;

  char cVar2;

  int iVar3;

  int iVar4;

  int iVar5;

  

                    /* 0xac494  6862  City.TCity.UpdateImages@23EDC2EF */

  iVar4 = (int)*(char *)(param_1[1] + 0x14);

  if (iVar4 == 3) {

    iVar5 = 0x14;

  }

  else if (((iVar4 == 5) || (iVar4 - 7U < 3)) || (iVar4 - 0xcU < 2)) {

    iVar5 = 0x28;

  }

  else {

    iVar5 = 0;

  }

  iVar3 = ImageLib_TImageSequenceList_GetImageSequence_23EDC2EF

                    (*(undefined4 *)(param_1[2] + 0x24),0);

  uVar1 = *(undefined4 *)(iVar3 + 0x10);

  cVar2 = (**(code **)(*param_1 + 0x150))();

  if (cVar2 == '\0') {

    if (*(char *)((int)param_1 + 0x45) < '\0') {

      cVar2 = ImageLib_TCustomImageLibrary_ImageExist_23EDC2EF(uVar1,iVar5 + 1);

      if (cVar2 == '\0') {

        iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(uVar1,1);

        param_1[0x1c] = iVar4;

      }

      else {

        iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(uVar1,iVar5 + 1);

        param_1[0x1c] = iVar4;

      }

    }

    else {

      iVar3 = AoWE_TRaceResourceList_GetRaceResource_23EDC2EF

                        (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSSet_678301D7_558e92e8 + 0x54),

                         (int)*(char *)((int)param_1 + 0x45));

      if (iVar3 != 0) {

        iVar4 = AoWE_TRaceResource_GetCityImage_23EDC2EF

                          (iVar3,*(undefined1 *)(param_1[2] + 0x78),iVar4,

                           (int)*(char *)(param_1[1] + 0x12));

        param_1[0x1c] = iVar4;

      }

      if (param_1[0x1c] == 0) {

        cVar2 = ImageLib_TCustomImageLibrary_ImageExist_23EDC2EF(uVar1,iVar5);

        if (cVar2 == '\0') {

          iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(uVar1,0);

          param_1[0x1c] = iVar4;

        }

        else {

          iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(uVar1,iVar5);

          param_1[0x1c] = iVar4;

        }

      }

    }

    if (((char)param_1[0x13] == '\0') || ((char)param_1[0x13] == '\x03')) {

      param_1[0x1a] = 0;

      param_1[0x1b] = 0;

    }

    else {

      iVar5 = (uint)*(byte *)(param_1 + 0x13) * 2 + iVar5;

      cVar2 = ImageLib_TCustomImageLibrary_ImageExist_23EDC2EF(uVar1,iVar5);

      if (cVar2 == '\0') {

        iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(uVar1,(uint)*(byte *)(param_1 + 0x13) * 2)

        ;

        param_1[0x1b] = iVar4;

        iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF

                          (uVar1,(uint)*(byte *)(param_1 + 0x13) * 2 + 1);

        param_1[0x1a] = iVar4;

      }

      else {

        iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(uVar1,iVar5);

        param_1[0x1b] = iVar4;

        iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(uVar1,iVar5 + 1);

        param_1[0x1a] = iVar4;

      }

    }

  }

  else {

    param_1[0x1c] = 0;

    param_1[0x1a] = 0;

    param_1[0x1b] = 0;

  }

  (**(code **)(*param_1 + 0x1fc))();

  return;

}




