
void FUN_0043e420(int param_1)

{
  code *pcVar1;
  char cVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined4 uVar6;
  
  pcVar1 = AoWE_AoWHSMap_6B957A58_exref;
  iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                     (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
  iVar4 = (**(code **)(**(int **)(iVar3 + 0x1c) + 0x84))
                    (*(int **)(iVar3 + 0x1c),*(undefined4 *)(param_1 + 0x2b0));
  iVar3 = iVar4 + -1;
  if (iVar3 < 0) {
    iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(*(int *)pcVar1 + 0x140),(int)*(char *)(*(int *)pcVar1 + 0xa5)
                      );
    iVar3 = (**(code **)(**(int **)(iVar3 + 0x1c) + 0x54))();
    iVar3 = iVar3 + -1;
  }
  while (iVar4 != iVar3) {
    iVar5 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(*(int *)pcVar1 + 0x140),(int)*(char *)(*(int *)pcVar1 + 0xa5)
                      );
    uVar6 = AoWE_TPlayerStructureList_GetStructure_23EDC2EF(*(undefined4 *)(iVar5 + 0x1c),iVar3);
    cVar2 = System__IsClass_51F89FF7(uVar6,City__TCity_01CDE5D7_exref);
    if (cVar2 != '\0') {
      iVar3 = iVar4 + 1;
      FUN_0043af24(param_1,uVar6);
    }
    iVar3 = iVar3 + -1;
    if (iVar3 < 0) {
      iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)pcVar1 + 0x140),
                         (int)*(char *)(*(int *)pcVar1 + 0xa5));
      iVar3 = (**(code **)(**(int **)(iVar3 + 0x1c) + 0x54))();
      iVar3 = iVar3 + -1;
    }
  }
  return;
}

