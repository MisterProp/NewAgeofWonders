
int Farm_FarmIncome_82863DB4(char param_1)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  int iVar5;
  int local_14;
  
                    /* 0xb2ebc  7098  Farm.FarmIncome@82863DB4 */
  if (*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 == 0) {
    local_14 = -1;
  }
  else {
    iVar1 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),
                       (int)param_1);
    local_14 = 0;
    iVar2 = (**(code **)(**(int **)(iVar1 + 0x1c) + 0x54))();
    if (-1 < iVar2 + -1) {
      iVar5 = 0;
      do {
        piVar3 = (int *)AoWE_TPlayerStructureList_GetStructure_23EDC2EF
                                  (*(undefined4 *)(iVar1 + 0x1c),iVar5);
        iVar4 = (**(code **)(*piVar3 + 0x24))();
        if ((iVar4 == 0x2036e) && ((char)piVar3[0xc] == param_1)) {
          local_14 = local_14 + (uint)*(byte *)(piVar3 + 0xf);
        }
        iVar5 = iVar5 + 1;
        iVar2 = iVar2 + -1;
      } while (iVar2 != 0);
    }
  }
  return local_14;
}

