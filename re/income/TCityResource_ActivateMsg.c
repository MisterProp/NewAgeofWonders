

void City_TCityResource_ActivateMsg_23EDC2EF(int param_1)



{

  char cVar1;

  int iVar2;

  int iVar3;

  

                    /* 0xadee8  6819  City.TCityResource.ActivateMsg@23EDC2EF */

  *(undefined1 *)(param_1 + 0x78) = 0;

  iVar2 = *(int *)(*(int *)(param_1 + 0x28) + 4);

  if (-1 < iVar2 + -1) {

    iVar3 = 0;

    do {

      cVar1 = HSEngine_TTerrainList_GetTerrain_23EDC2EF(*(undefined4 *)(param_1 + 0x28),iVar3);

      if (-1 < cVar1) {

        *(char *)(param_1 + 0x78) = *(char *)(param_1 + 0x78) + '\x01';

      }

      iVar3 = iVar3 + 1;

      iVar2 = iVar2 + -1;

    } while (iVar2 != 0);

  }

  return;

}




