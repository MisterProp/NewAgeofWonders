

void Crops_TPlayerCropStructure_LinkToCrops_23EDC2EF(int *param_1)



{

  int iVar1;

  char cVar2;

  int *piVar3;

  int iVar4;

  int iVar5;

  int iStack_14;

  int iStack_10;

  

                    /* 0xa686c  6724  Crops.TPlayerCropStructure.LinkToCrops@23EDC2EF */

  cVar2 = (**(code **)(*param_1 + 0x84))();

  if (cVar2 == '\0') {

    iVar1 = *(int *)(param_1[1] + 4);

    iVar5 = *(int *)(*(int *)(param_1[2] + 0x74) + 8) + -1;

    if (-1 < iVar5) {

      do {

        HSEngine_CenterHNtoHX_A52F39E4

                  ((int)(char)param_1[4],(int)*(char *)((int)param_1 + 0x11),

                   *(undefined2 *)(*(int *)(*(int *)(param_1[2] + 0x74) + 4) + iVar5 * 2),&iStack_10

                   ,&iStack_14);

        if ((((-1 < iStack_14) && (iStack_14 < *(int *)(iVar1 + 0xc))) && (-1 < iStack_10)) &&

           (iStack_10 < *(int *)(iVar1 + 0x10))) {

          piVar3 = (int *)HSEngine_TMapLevel_GetField_23EDC2EF(iVar1,iStack_14,iStack_10);

          iVar4 = (**(code **)(*piVar3 + 0x7c))(piVar3,0x2036a);

          if (iVar4 != 0) {

            Crops_TCrop_SetCropOwner_23EDC2EF(iVar4,param_1);

          }

        }

        iVar5 = iVar5 + -1;

      } while (iVar5 != -1);

    }

  }

  return;

}




