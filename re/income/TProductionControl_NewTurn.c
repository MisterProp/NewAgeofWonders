

void AoWE_TProductionControl_NewTurn_23EDC2EF(int *param_1,char param_2)



{

  char *pcVar1;

  char cVar2;

  int iVar3;

  undefined4 uVar4;

  

                    /* 0x5dd00  2326  AoWE.TProductionControl.NewTurn@23EDC2EF */

  if (((char)param_1[8] == '\x01') && (cVar2 = (**(code **)(*param_1 + 0x7c))(), param_2 == cVar2))

  {

    iVar3 = (**(code **)(*(int *)param_1[5] + 0x54))();

    if (iVar3 == 0) {

      *(undefined1 *)(param_1 + 8) = 0;

      return;

    }

    uVar4 = AoWE_TProductionQueue_GetItem_23EDC2EF(param_1[5],0);

    (**(code **)(*param_1 + 0x60))(param_1,uVar4);

  }

  else if (((char)param_1[8] == '\x02') && (param_2 == *(char *)(param_1[9] + 0x1d))) {

    if ('\0' < *(char *)(param_1[9] + 0x10)) {

      pcVar1 = (char *)(param_1[9] + 0x10);

      *pcVar1 = *pcVar1 + -1;

    }

    if (*(char *)(param_1[9] + 0x10) == '\0') {

      iVar3 = AoWE_TProductionQueue_GetItem_23EDC2EF(param_1[5],0);

      if (*(int *)(iVar3 + 8) == 1) {

        cVar2 = (**(code **)(*param_1 + 0x90))();

        if (cVar2 != '\0') {

          iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF

                            (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),(int)param_2);

          if ((*(char *)(iVar3 + 0xa7) != '\0') && (*(int *)(param_1[9] + 0x20) != 0)) {

            AoWE_TProductionControl_GroupProductionProduced_23EDC2EF

                      (param_1,*(undefined4 *)(param_1[9] + 0x24),*(undefined4 *)(param_1[9] + 0x20)

                      );

          }

          (**(code **)(*(int *)param_1[9] + 0x30))();

          AoWE_TProductionControl_ProductionDone_23EDC2EF(param_1);

        }

      }

      else if (*(int *)(iVar3 + 8) == 2) {

        (**(code **)(*param_1 + 0x94))();

        (**(code **)(*(int *)param_1[9] + 0x30))();

        AoWE_TProductionControl_ProductionDone_23EDC2EF(param_1);

      }

    }

  }

  return;

}




