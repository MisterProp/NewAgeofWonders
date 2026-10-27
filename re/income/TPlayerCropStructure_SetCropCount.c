

void Crops_TPlayerCropStructure_SetCropCount_23EDC2EF(int *param_1,int param_2)



{

  int iVar1;

  char cVar2;

  int *piVar3;

  int *piVar4;

  int iVar5;

  undefined4 uVar6;

  int iVar7;

  uint uStack_20;

  uint uStack_1c;

  int local_18;

  int iStack_14;

  

                    /* 0xa65cc  6727  Crops.TPlayerCropStructure.SetCropCount@23EDC2EF */

  if (((*(byte *)(param_1 + 3) & 8) != 0) && (*(char *)((int)param_1 + 0x3d) == '\0')) {

    *(undefined1 *)((int)param_1 + 0x3d) = 1;

    iVar1 = *(int *)(param_1[1] + 4);

    local_18 = System_TObject_Create_23EDC2EF(Classes__TList_9F2060BF_exref,1);

    piVar3 = (int *)System_TObject_Create_23EDC2EF(Engine__TWordList_09D10B8A_exref,1);

    (**(code **)(*piVar3 + 8))(piVar3,*(undefined4 *)(param_1[2] + 0x74));

    iVar7 = piVar3[2] + -1;

    if (-1 < iVar7) {

      do {

        HSEngine_CenterHNtoHX_A52F39E4

                  ((int)(char)param_1[4],(int)*(char *)((int)param_1 + 0x11),

                   *(undefined2 *)(piVar3[1] + iVar7 * 2),&uStack_1c,&uStack_20);

        if (((((int)uStack_20 < 0) || (*(int *)(iVar1 + 0xc) <= (int)uStack_20)) ||

            ((int)uStack_1c < 0)) || (*(int *)(iVar1 + 0x10) <= (int)uStack_1c)) {

          Engine_TWordList_Delete_23EDC2EF(piVar3,iVar7);

        }

        else {

          piVar4 = (int *)HSEngine_TMapLevel_GetField_23EDC2EF(iVar1,uStack_20,uStack_1c);

          iStack_14 = (**(code **)(*piVar4 + 0x80))(piVar4,0x2036a);

          if ((iStack_14 != 0) ||

             (cVar2 = (**(code **)(*param_1 + 0x224))(param_1,piVar4), cVar2 == '\0')) {

            if ((iStack_14 != 0) && (param_1 == *(int **)(iStack_14 + 0x1c))) {

              Classes_TList_Add_23EDC2EF(local_18,iStack_14);

            }

            Engine_TWordList_Delete_23EDC2EF(piVar3,iVar7);

          }

        }

        iVar7 = iVar7 + -1;

      } while (iVar7 != -1);

    }

    if (*(int *)(local_18 + 8) < param_2) {

      piVar4 = *(int **)(*(int *)(*(int *)(param_1[2] + 0x18) + 0x38) + 0x24);

      iVar7 = (**(code **)(*piVar4 + 0x7c))(piVar4,0x2036b);

      if (iVar7 != 0) {

        while ((*(int *)(local_18 + 8) != param_2 && (piVar3[2] != 0))) {

          iVar5 = AoWE_TAoWHSMap_Random_23EDC2EF

                            (*(undefined4 *)PTR_AoWE_AoWHSMap_6B957A58_558e9494,piVar3[2]);

          HSEngine_CenterHNtoHX_A52F39E4

                    ((int)(char)param_1[4],(int)*(char *)((int)param_1 + 0x11),

                     *(undefined2 *)(piVar3[1] + iVar5 * 2),&uStack_1c,&uStack_20);

          iStack_14 = Crops_TCropResource_PlaceCrop_23EDC2EF

                                (iVar7,uStack_20 & 0xff,uStack_1c & 0xff,param_1,

                                 *(undefined1 *)(iVar1 + 8));

          if (iStack_14 != 0) {

            Classes_TList_Add_23EDC2EF(local_18,iStack_14);

            Crops_TCrop_SetCropOwner_23EDC2EF(iStack_14,param_1);

          }

          Engine_TWordList_Delete_23EDC2EF(piVar3,iVar5);

        }

      }

    }

    else if (param_2 < *(int *)(local_18 + 8)) {

      while ((iVar1 = *(int *)(local_18 + 8), iVar1 != param_2 && (iVar1 != 0))) {

        uVar6 = AoWE_TAoWHSMap_Random_23EDC2EF

                          (*(undefined4 *)PTR_AoWE_AoWHSMap_6B957A58_558e9494,iVar1);

        piVar4 = (int *)Classes_TList_Get_23EDC2EF(local_18,uVar6);

        (**(code **)(*piVar4 + -4))(piVar4,1);

        Classes_TList_Delete_23EDC2EF(local_18,uVar6);

      }

    }

    System_TObject_Free_23EDC2EF(local_18);

    System_TObject_Free_23EDC2EF(piVar3);

    *(undefined1 *)((int)param_1 + 0x3d) = 0;

    (**(code **)(*param_1 + 0x23c))();

  }

  return;

}




