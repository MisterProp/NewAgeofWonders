

undefined4 HSEngine_TTerrainList_CreateSurroundingHNTable_23EDC2EF(int param_1)



{

  char cVar1;

  undefined4 uVar2;

  int iVar3;

  int iVar4;

  int iVar5;

  int iVar6;

  undefined4 local_38;

  undefined4 local_34;

  undefined1 local_30 [8];

  int local_28;

  undefined4 local_20;

  undefined4 local_1c;

  

                    /* 0x51e8  534  HSEngine.TTerrainList.CreateSurroundingHNTable@23EDC2EF */

  uVar2 = System_TObject_Create_23EDC2EF(Engine__TWordList_09D10B8A_exref,1);

  iVar5 = *(int *)(param_1 + 4);

  if (-1 < iVar5 + -1) {

    iVar6 = 0;

    do {

      cVar1 = HSEngine_TTerrainList_GetTerrain_23EDC2EF(param_1,iVar6);

      if (-1 < cVar1) {

        HSEngine_HNtoHX_56E4633A(iVar6,&local_38,&local_34);

        HSEngine_InitHNWalk_3DC7E9B4(local_30,local_38,local_34,1);

        while (local_28 != 7) {

          iVar3 = HSEngine_HXtoHN_500B4C01(local_20,local_1c);

          if (iVar3 < *(int *)(param_1 + 4)) {

            if (iVar3 < *(int *)(param_1 + 4)) {

              cVar1 = HSEngine_TTerrainList_GetTerrain_23EDC2EF(param_1,iVar3);

              if (cVar1 < '\0') goto LAB_55605266;

            }

          }

          else {

LAB_55605266:

            iVar4 = Engine_TWordList_IndexOf_23EDC2EF(uVar2,iVar3);

            if (iVar4 == -1) {

              Engine_TWordList_Add_23EDC2EF(uVar2,iVar3);

            }

          }

          HSEngine_WalkToNextHN_77BE26A7(local_30);

        }

      }

      iVar6 = iVar6 + 1;

      iVar5 = iVar5 + -1;

    } while (iVar5 != 0);

  }

  return uVar2;

}




