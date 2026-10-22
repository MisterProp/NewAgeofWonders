
void FUN_0044975c(int param_1)

{
  int *piVar1;
  undefined1 *puVar2;
  char cVar3;
  int iVar4;
  int iVar5;
  int *piVar6;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_28;
  undefined1 *puStack_24;
  undefined1 *puStack_20;
  undefined4 local_10;
  int local_c;
  int local_8;
  
  puStack_20 = &stack0xfffffffc;
  local_10 = 0;
  puStack_24 = &LAB_00449886;
  uStack_28 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_28;
  local_8 = param_1;
  (**(code **)(**(int **)(*(int *)(param_1 + 200) + 0x114) + 0x40))();
  (**(code **)(**(int **)(*(int *)(local_8 + 0xd0) + 0x114) + 0x40))();
  iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                     (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
  iVar4 = (**(code **)(**(int **)(iVar4 + 0x1c) + 0x54))();
  if (-1 < iVar4 + -1) {
    local_c = 0;
    do {
      iVar5 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                         (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
      piVar6 = (int *)AoWE_TPlayerStructureList_GetStructure_23EDC2EF
                                (*(undefined4 *)(iVar5 + 0x1c),local_c);
      cVar3 = System__IsClass_51F89FF7(piVar6,City__TCity_01CDE5D7_exref);
      if (cVar3 == '\0') {
        (**(code **)(*piVar6 + 0x104))(piVar6,&local_10);
        piVar1 = *(int **)(*(int *)(local_8 + 200) + 0x114);
        (**(code **)(*piVar1 + 0x38))(piVar1,local_10,piVar6);
        (**(code **)(*piVar6 + 0x1b4))(piVar6,&local_10);
        piVar6 = *(int **)(*(int *)(local_8 + 0xd0) + 0x114);
        (**(code **)(*piVar6 + 0x34))(piVar6,local_10);
      }
      local_c = local_c + 1;
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
  }
  puVar2 = puStack_20;
  *in_FS_OFFSET = uStack_28;
  puStack_20 = &LAB_0044988d;
  puStack_24 = (undefined1 *)0x449885;
  System__LStrClr_40929B27(&local_10,uStack_28,puVar2);
  return;
}

