
undefined4 Terrain_TTerrainMO_FindTerrainResource_23EDC2EF(int *param_1,char param_2)

{
  int *piVar1;
  char cVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined4 uVar7;
  int iVar8;
  int iVar9;
  
                    /* 0x1652c  869  Terrain.TTerrainMO.FindTerrainResource@23EDC2EF */
  iVar3 = System_TObject_Create_23EDC2EF(Classes__TList_9F2060BF_exref,1);
  iVar4 = (**(code **)(*param_1 + 0x108))(param_1,*(undefined4 *)(param_1[2] + 0x18));
  iVar4 = *(int *)(iVar4 + 8);
  iVar9 = *(int *)(iVar4 + 8);
  if (-1 < iVar9 + -1) {
    iVar8 = 0;
    do {
      piVar1 = *(int **)(*(int *)(iVar4 + 4) + iVar8 * 4);
      iVar5 = (**(code **)(*piVar1 + 0x24))();
      iVar6 = (**(code **)(*(int *)param_1[2] + 0x24))();
      if ((((iVar5 == iVar6) && ((char)piVar1[0x11] == *(char *)(param_1[2] + 0x44))) &&
          (*(char *)((int)piVar1 + 0x45) == param_2)) && (piVar1 != (int *)param_1[2])) {
        cVar2 = HSEngine_TTerrainList_CompareTerrain_23EDC2EF
                          (piVar1[10],*(undefined4 *)(param_1[2] + 0x28),1);
        if (cVar2 != '\0') {
          Classes_TList_Add_23EDC2EF(iVar3,piVar1);
        }
      }
      iVar8 = iVar8 + 1;
      iVar9 = iVar9 + -1;
    } while (iVar9 != 0);
  }
  if (*(int *)(iVar3 + 8) == 0) {
    uVar7 = 0;
  }
  else {
    iVar4 = System__RandInt_51F89FF7(*(int *)(iVar3 + 8));
    uVar7 = *(undefined4 *)(*(int *)(iVar3 + 4) + iVar4 * 4);
  }
  System_TObject_Free_23EDC2EF(iVar3);
  return uVar7;
}

