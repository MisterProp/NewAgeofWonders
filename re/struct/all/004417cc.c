
void FUN_004417cc(int param_1)

{
  code *pcVar1;
  char cVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int *piVar6;
  
  pcVar1 = AoWE_AoWHSMap_6B957A58_exref;
  iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                     (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
  iVar4 = (**(code **)(**(int **)(iVar3 + 0x1c) + 0x84))
                    (*(int **)(iVar3 + 0x1c),*(undefined4 *)(param_1 + 0x1e8));
  iVar3 = iVar4 + 1;
  iVar5 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                    (*(undefined4 *)(*(int *)pcVar1 + 0x140),(int)*(char *)(*(int *)pcVar1 + 0xa5));
  iVar5 = (**(code **)(**(int **)(iVar5 + 0x1c) + 0x54))();
  if (iVar5 <= iVar3) {
    iVar3 = 0;
  }
  while (iVar4 != iVar3) {
    iVar5 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(*(int *)pcVar1 + 0x140),(int)*(char *)(*(int *)pcVar1 + 0xa5)
                      );
    piVar6 = (int *)AoWE_TPlayerStructureList_GetStructure_23EDC2EF
                              (*(undefined4 *)(iVar5 + 0x1c),iVar3);
    cVar2 = System__IsClass_51F89FF7(piVar6,PrdPlace__TProductionPlace_9A13A4CB_exref);
    if ((cVar2 != '\0') && (cVar2 = (**(code **)(*piVar6 + 0x230))(), cVar2 == '\b')) {
      iVar3 = iVar4 + -1;
      FUN_0043f1f4(param_1,piVar6);
    }
    iVar3 = iVar3 + 1;
    iVar5 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(*(int *)pcVar1 + 0x140),(int)*(char *)(*(int *)pcVar1 + 0xa5)
                      );
    iVar5 = (**(code **)(**(int **)(iVar5 + 0x1c) + 0x54))();
    if (iVar5 <= iVar3) {
      iVar3 = 0;
    }
  }
  return;
}

