

void City_TCity_SetUpgradeLevel_23EDC2EF(int param_1,uint param_2,undefined4 param_3)



{

  undefined1 uVar1;

  int iVar2;

  int iVar3;

  undefined4 uVar4;

  int iVar5;

  char cVar6;

  int iVar7;

  

                    /* 0xac75c  6861  City.TCity.SetUpgradeLevel@23EDC2EF */

  iVar2 = City_TCity_GetSize_23EDC2EF(param_1,param_2,param_3,param_3);

  if (iVar2 < (int)(param_2 & 0xff)) {

    param_2 = City_TCity_GetSize_23EDC2EF(param_1);

  }

  cVar6 = (char)param_2;

  if (cVar6 == '\0') {

    cVar6 = '\x01';

  }

  *(char *)(param_1 + 0x4d) = cVar6;

  if (-1 < *(char *)(param_1 + 0x45)) {

    iVar2 = AoWE_TRaceResourceList_GetRaceResource_23EDC2EF

                      (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSSet_678301D7_558e92e8 + 0x54),

                       (int)*(char *)(param_1 + 0x45));

    iVar7 = *(int *)(*(int *)(iVar2 + 0x38) + 8) + -1;

    if (-1 < iVar7) {

      do {

        iVar3 = Engine_TIntegerList_Get_23EDC2EF(*(undefined4 *)(iVar2 + 0x38),iVar7);

        if (iVar3 != -1) {

          uVar4 = Engine_TIntegerList_Get_23EDC2EF(*(undefined4 *)(iVar2 + 0x38),iVar7);

          iVar3 = AoWE_TUnitProductionList_IndexOf_23EDC2EF

                            (*(undefined4 *)(*(int *)(param_1 + 0x50) + 0x1c),uVar4);

          if (iVar3 == -1) {

            uVar4 = Engine_TIntegerList_Get_23EDC2EF(*(undefined4 *)(iVar2 + 0x38),iVar7);

            iVar3 = AoWE_TUnitResourceList_GetUnitResource_23EDC2EF

                              (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSSet_678301D7_558e92e8 + 0x5c),

                               uVar4);

            cVar6 = *(char *)(iVar3 + 0x2f);

            uVar4 = City_TCity_GetSize_23EDC2EF(param_1);

            iVar3 = City_UnitLevelToCityLevel_500B4C01((int)cVar6,uVar4);

            if ((iVar3 != -1) && (iVar3 <= (int)(uint)*(byte *)(param_1 + 0x4d))) {

              if (iVar3 == 1) {

                if (3 < *(int *)(*(int *)(iVar2 + 0x40) + 8)) {

                  iVar3 = Engine_TIntegerList_Get_23EDC2EF(*(undefined4 *)(iVar2 + 0x38),iVar7);

                  iVar5 = Engine_TIntegerList_Get_23EDC2EF(*(undefined4 *)(iVar2 + 0x40),3);

                  if (iVar3 == iVar5) goto LAB_557ac84d;

                }

                uVar1 = 1;

              }

              else {

LAB_557ac84d:

                uVar1 = 0;

              }

              uVar4 = Engine_TIntegerList_Get_23EDC2EF(*(undefined4 *)(iVar2 + 0x38),iVar7);

              AoWE_TUnitProductionList_Add_23EDC2EF

                        (*(undefined4 *)(*(int *)(param_1 + 0x50) + 0x1c),uVar4,uVar1);

            }

          }

        }

        iVar7 = iVar7 + -1;

      } while (iVar7 != -1);

    }

    DAT_558fa0ec = param_1;

    Classes_TList_Sort_23EDC2EF

              (*(undefined4 *)(*(int *)(*(int *)(param_1 + 0x50) + 0x1c) + 8),FUN_557ac624);

  }

  return;

}




