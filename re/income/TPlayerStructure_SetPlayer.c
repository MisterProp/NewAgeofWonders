

void AoWE_TPlayerStructure_SetPlayer_23EDC2EF(int *param_1,char param_2)



{

  char cVar1;

  char cVar2;

  int iVar3;

  undefined4 uVar4;

  int iVar5;

  

                    /* 0x60d08  2207  AoWE.TPlayerStructure.SetPlayer@23EDC2EF */

  if (param_2 != (char)param_1[0xc]) {

    cVar1 = (**(code **)(*param_1 + 0x150))();

    if ((cVar1 != '\0') && (cVar1 = AoWE_TStructure_GetBuilding_23EDC2EF(param_1), cVar1 == '\0')) {

      return;

    }

    *(undefined1 *)((int)param_1 + 0x31) = 0;

    if ((char)param_1[0xc] != -1) {

      iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF

                        (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),(int)(char)param_1[0xc]);

      (**(code **)(**(int **)(iVar3 + 0x1c) + 0x6c))(*(int **)(iVar3 + 0x1c),param_1);

    }

    if (0 < *(int *)(AoWE_AoWHSMap_6B957A58 + 0x174)) {

      (**(code **)(*param_1 + 0x214))(param_1,(char)param_1[0xc],param_2);

    }

    *(char *)(param_1 + 0xc) = param_2;

    (**(code **)(*param_1 + 0x204))();

    AoWE_TPlayerStructure_UpdatePlayerSeen_23EDC2EF(param_1);

    (**(code **)(*param_1 + 0x208))();

    cVar1 = (**(code **)(*param_1 + 0x7c))();

    iVar3 = (int)cVar1;

    uVar4 = HSEngine_HNtoRad_DB882642(*(undefined4 *)(*(int *)(param_1[2] + 0x28) + 4));

    cVar1 = (**(code **)(*param_1 + 0x78))();

    cVar2 = (**(code **)(*param_1 + 0x74))();

    AoWE_TAbstractAoWHSMap_ScannerIconsChangedRad_23EDC2EF

              (AoWE_AoWHSMap_6B957A58,(int)cVar2,(int)cVar1,uVar4,iVar3);

    (**(code **)(*param_1 + 0x1fc))();

    AoWE_TStructure_FogChanged_23EDC2EF(param_1);

    cVar1 = (**(code **)(*param_1 + 0x7c))();

    iVar3 = (int)cVar1;

    cVar1 = (**(code **)(*param_1 + 0x78))();

    iVar5 = (int)cVar1;

    cVar1 = (**(code **)(*param_1 + 0x74))();

    AoWE_TAoWHSMap_NotifyPlayerChanged_23EDC2EF(AoWE_AoWHSMap_6B957A58,(int)cVar1,iVar5,iVar3);

    if ((char)param_1[0xc] != -1) {

      iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF

                        (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),(int)(char)param_1[0xc]);

      (**(code **)(**(int **)(iVar3 + 0x1c) + 0x70))(*(int **)(iVar3 + 0x1c),param_1);

    }

    if (('\0' < (char)param_1[0xc]) && (*(char *)(AoWE_AoWHSMap_6B957A58 + 0x14d) == '\x01')) {

      iVar3 = (**(code **)(**(int **)(AoWE_AoWHSMap_6B957A58 + 0x140) + 0x54))();

      iVar3 = iVar3 + -1;

      if (0 < iVar3) {

        iVar5 = 1;

        do {

          cVar1 = Engine_TBitList_Get_23EDC2EF(param_1[0xd],iVar5);

          if ((cVar1 != '\0') &&

             ((iVar5 == (char)param_1[0xc] ||

              ((*(char *)(AoWE_AoWHSMap_6B957A58 + 0x14b) != '\0' &&

               (cVar1 = AoWE_TPlayerList_GetDiplomaticRelation_23EDC2EF

                                  (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),

                                   (char)param_1[0xc],iVar5), cVar1 == '\x02')))))) {

            AoWE_TPlayerList_GetPlayers_23EDC2EF

                      (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),iVar5);

            AoWE_TPlayer_CheckCaptureLocations_23EDC2EF();

          }

          iVar5 = iVar5 + 1;

          iVar3 = iVar3 + -1;

        } while (iVar3 != 0);

      }

    }

    (**(code **)(*param_1 + 0x1c8))();

  }

  return;

}




