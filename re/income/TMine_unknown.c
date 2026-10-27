

void Mine_TMine_UpdateAITarget_23EDC2EF(int *param_1,char *param_2,int *param_3)



{

  char cVar1;

  int iVar2;

  int iVar3;

  

                    /* 0xb4498  7171  Mine.TMine.UpdateAITarget@23EDC2EF */

  cVar1 = (**(code **)(*param_1 + 0x150))();

  if ((cVar1 == '\0') || (cVar1 = AoWE_TStructure_GetBuilding_23EDC2EF(param_1), cVar1 != '\0')) {

    iVar3 = *param_3;

    if (param_2[2] == '\0') {

      if ((*param_2 != (char)param_1[0xc]) &&

         (cVar1 = (**(code **)(*param_1 + 0x150))(), cVar1 == '\0')) {

        if ((char)param_1[0xc] == '\0') {

          iVar3 = 0x32;

        }

        else {

          cVar1 = AoWE_ValidAttackRelation_C8BA104C(*param_2,param_2[1],(char)param_1[0xc],0);

          if (cVar1 != '\0') {

            iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF

                              (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),

                               (int)*param_2);

            iVar3 = (int)*(uint *)(iVar2 + 0xd0) >> 1;

            if (iVar3 < 0) {

              iVar3 = iVar3 + (uint)((*(uint *)(iVar2 + 0xd0) & 1) != 0);

            }

            iVar3 = 0x32 - iVar3;

            if (iVar3 < 0x19) {

              iVar3 = 0x19;

            }

            if (0x4b < iVar3) {

              iVar3 = 0x4b;

            }

          }

        }

      }

    }

    else if ((param_2[2] == '\x06') && (*param_2 == (char)param_1[0xc])) {

      *param_3 = 0x14;

    }

    if (*param_3 < iVar3) {

      *param_3 = iVar3;

    }

  }

  return;

}




