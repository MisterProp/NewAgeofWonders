
void FUN_004575e0(int param_1,undefined4 param_2,undefined4 *param_3)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  undefined4 *puVar5;
  int local_50;
  undefined4 local_4c [15];
  
  puVar5 = local_4c;
  for (iVar3 = 0xf; iVar3 != 0; iVar3 = iVar3 + -1) {
    *puVar5 = *param_3;
    param_3 = param_3 + 1;
    puVar5 = puVar5 + 1;
  }
  local_50 = 0;
  if (*(int *)AoWE_AoWHSMap_6B957A58_exref != 0) {
    iVar3 = (**(code **)(**(int **)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140) + 0x54))();
    if (-1 < iVar3 + -1) {
      iVar4 = 0;
      do {
        iVar1 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                          (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),iVar4);
        if (*(char *)(iVar1 + 0x45) == '\x01') {
          iVar1 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                            (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),iVar4);
          iVar1 = (int)*(char *)(iVar1 + 0xa5);
          if (iVar1 == -1) {
            iVar1 = 0xc;
          }
          puVar5 = local_4c;
          uVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                            (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0x88),iVar1 + 0x20);
          ImageLib_TLibraryImage_ShowClipped_23EDC2EF
                    (uVar2,*(int *)(*(int *)(param_1 + 0x94) + 0x84) + 3 + local_50 * 5,
                     *(int *)(*(int *)(param_1 + 0x94) + 0x88) + 2,puVar5);
          if (*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x239) == '\x01') {
            if (iVar4 == *(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa4)) {
              (**(code **)(**(int **)(param_1 + 0x98) + 0x58))
                        (*(int **)(param_1 + 0x98),
                         *(int *)(*(int *)(param_1 + 0x94) + 0x84) + 2 + local_50 * 5);
              (**(code **)(**(int **)(param_1 + 0x98) + 0x5c))
                        (*(int **)(param_1 + 0x98),*(int *)(*(int *)(param_1 + 0x94) + 0x88) + 0xb);
            }
          }
          else {
            iVar1 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                              (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),iVar4);
            if (*(char *)(iVar1 + 0x2c) != '\0') {
              puVar5 = local_4c;
              uVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0x88),0x32);
              ImageLib_TLibraryImage_ShowClipped_23EDC2EF
                        (uVar2,*(int *)(*(int *)(param_1 + 0x94) + 0x84) + 2 + local_50 * 5,
                         *(int *)(*(int *)(param_1 + 0x94) + 0x88) + 0xb,puVar5);
            }
          }
          local_50 = local_50 + 1;
        }
        iVar4 = iVar4 + 1;
        iVar3 = iVar3 + -1;
      } while (iVar3 != 0);
    }
  }
  return;
}

