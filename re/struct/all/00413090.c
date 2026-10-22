
void FUN_00413090(int param_1)

{
  int *piVar1;
  char cVar2;
  undefined4 uVar3;
  int iVar4;
  int in_stack_00000008;
  
  if (in_stack_00000008 == 0) {
    (**(code **)(**(int **)(param_1 + 0x50) + 0x70))(*(int **)(param_1 + 0x50),0x8fa9a0);
  }
  else {
    piVar1 = *(int **)(*(int *)(param_1 + 0x50) + 0x114);
    uVar3 = (**(code **)(*piVar1 + 0x18))(piVar1,in_stack_00000008);
    cVar2 = NetworkE_TPlayerCommunicationControl_IndexOfSystemID_23EDC2EF
                      (*(undefined4 *)
                        (*(int *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x240) + 0x1c),uVar3);
    if ('\0' < cVar2) {
      iVar4 = (**(code **)(**(int **)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140) + 0x54))();
      if (cVar2 < iVar4) {
        iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                          (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),(int)cVar2)
        ;
        (**(code **)(**(int **)(param_1 + 0x50) + 0x70))
                  (*(int **)(param_1 + 0x50),
                   *(undefined4 *)(AoWE_RaceColors_CF011141_exref + *(char *)(iVar4 + 0xa5) * 4));
      }
    }
  }
  return;
}

