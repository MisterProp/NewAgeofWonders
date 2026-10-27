

byte AoWE_TPlayerStructure_UpdatePlayer_23EDC2EF(int *param_1)



{

  int iVar1;

  char cVar2;

  int iVar3;

  int *piVar4;

  int iVar5;

  bool bVar6;

  char local_34;

  undefined1 auStack_2c [8];

  int iStack_24;

  int iStack_1c;

  int iStack_18;

  

                    /* 0x60980  2213  AoWE.TPlayerStructure.UpdatePlayer@23EDC2EF */

  local_34 = -1;

  bVar6 = false;

  iVar3 = (**(code **)(*param_1 + 0x54))();

  iVar1 = *(int *)(param_1[1] + 4);

  HSEngine_InitHNWalk_3DC7E9B4

            (auStack_2c,(int)(char)param_1[4],(int)*(char *)((int)param_1 + 0x11),0);

  while( true ) {

    if ((iVar3 <= iStack_24) || (bVar6 != false)) break;

    cVar2 = HSEngine_TTerrainList_GetTerrain_23EDC2EF(*(undefined4 *)(param_1[2] + 0x28),iStack_24);

    if (((-1 < cVar2) &&

        (((-1 < iStack_1c && (-1 < iStack_18)) && (iStack_1c < *(int *)(iVar1 + 0xc))))) &&

       (iStack_18 < *(int *)(iVar1 + 0x10))) {

      piVar4 = (int *)HSEngine_TMapLevel_GetField_23EDC2EF(iVar1,iStack_1c,iStack_18);

      iVar5 = (**(code **)(*piVar4 + 0x80))(piVar4,0x20217);

      if (iVar5 != 0) {

        if (local_34 == -1) {

          local_34 = *(char *)(*(int *)(iVar5 + 0x1c) + 0x12);

        }

        else {

          bVar6 = *(char *)(*(int *)(iVar5 + 0x1c) + 0x12) != local_34;

        }

      }

    }

    HSEngine_WalkToNextHN_77BE26A7(auStack_2c);

  }

  if (local_34 != -1) {

    if (bVar6 == false) {

      (**(code **)(*param_1 + 0x1f0))(param_1,local_34);

    }

    else {

      (**(code **)(*param_1 + 0x1f0))(param_1,0xffffffff);

    }

  }

  return bVar6 ^ 1;

}




