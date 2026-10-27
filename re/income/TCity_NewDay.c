

void City_TCity_NewDay_23EDC2EF(int *param_1)



{

  byte bVar1;

  undefined1 uVar2;

  undefined4 uVar3;

  int iVar4;

  

                    /* 0xab864  6872  City.TCity.NewDay@23EDC2EF */

  AoWE_TPlayerStructure_NewDay_23EDC2EF(param_1);

  if (*(char *)((int)param_1 + 0x45) < '\0') {

    bVar1 = *(byte *)((int)param_1 + 0x45);

    if (((char)bVar1 < -1) &&

       (*(char *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x11a) == '\x01')) {

      iVar4 = (char)((bVar1 ^ (char)bVar1 >> 7) - ((char)bVar1 >> 7)) + -3;

      if ((iVar4 < 0) ||

         (*(int *)(*(int *)(*(int *)(*(int *)PTR_AoWE_AoWEngine_7E705837_558e945c + 0x70) + 0x18) +

                  8) <= iVar4)) {

        uVar3 = AoWE_TRaceList_GetRandomRace_23EDC2EF

                          (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x144));

        (**(code **)(*param_1 + 0x244))(param_1,uVar3);

      }

      else {

        uVar3 = Engine_TByteList_Get_23EDC2EF

                          (*(undefined4 *)

                            (*(int *)(*(int *)PTR_AoWE_AoWEngine_7E705837_558e945c + 0x70) + 0x18),

                           iVar4);

        (**(code **)(*param_1 + 0x244))(param_1,uVar3);

      }

    }

    else {

      uVar3 = AoWE_TRaceList_GetRandomRace_23EDC2EF

                        (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x144));

      (**(code **)(*param_1 + 0x244))(param_1,uVar3);

    }

  }

  if (*(int *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x174) == 1) {

    (**(code **)(**(int **)(param_1[0x14] + 0x1c) + 0x30))();

    City_TCity_SetUpgradeLevel_23EDC2EF(param_1,*(undefined1 *)((int)param_1 + 0x4d));

    City_TCity_GenerateDefenders_23EDC2EF(param_1);

    if ((char)param_1[0x13] == '\x03') {

      uVar2 = AoWE_TAoWHSMap_Random_23EDC2EF(*(undefined4 *)PTR_AoWE_AoWHSMap_6B957A58_558e9494,3);

      *(undefined1 *)(param_1 + 0x13) = uVar2;

      City_TCity_UpdateImages_23EDC2EF(param_1);

    }

  }

  City_TCity_CheckRebellion_23EDC2EF(param_1);

  if (*(char *)(param_1[0x18] + 0x14) == '\x01') {

    City_TCityOverlayHS_SetMode_23EDC2EF(param_1[0x18],0);

  }

  return;

}




