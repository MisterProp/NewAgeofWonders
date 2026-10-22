
void FUN_00431920(int param_1)

{
  int iVar1;
  undefined1 *puVar2;
  char cVar3;
  int iVar4;
  undefined4 uVar5;
  int *piVar6;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_24;
  undefined1 *puStack_20;
  undefined1 *puStack_1c;
  undefined4 local_8;
  
  puStack_1c = &stack0xfffffffc;
  local_8 = 0;
  puStack_20 = &LAB_004319f3;
  uStack_24 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_24;
  FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,0);
  iVar1 = *(int *)(*(int *)(param_1 + 100) + 0x120);
  if (-1 < iVar1) {
    iVar4 = (**(code **)(**(int **)(*(int *)(param_1 + 100) + 0x114) + 0x14))();
    if (iVar1 < iVar4) {
      uVar5 = AoWE_TUnitControl_FindUnit_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xfc),
                         *(undefined4 *)(*(int *)(param_1 + 0x94) + 0xc));
      piVar6 = *(int **)(*(int *)(param_1 + 100) + 0x114);
      piVar6 = (int *)(**(code **)(*piVar6 + 0x18))
                                (piVar6,*(undefined4 *)(*(int *)(param_1 + 100) + 0x120));
      cVar3 = (**(code **)(*piVar6 + 0x50))(piVar6,uVar5,&local_8);
      if (cVar3 == '\0') {
        (**(code **)(**(int **)AoWE_AoWHSMap_6B957A58_exref + 0x114))
                  (*(int **)AoWE_AoWHSMap_6B957A58_exref,local_8);
      }
      else {
        cVar3 = (**(code **)(*piVar6 + 0x54))(piVar6,uVar5);
        if (cVar3 != '\0') {
          (**(code **)(**(int **)(param_1 + 0x5c) + 0x6c))(*(int **)(param_1 + 0x5c),0);
        }
      }
    }
  }
  puVar2 = puStack_1c;
  *in_FS_OFFSET = uStack_24;
  puStack_1c = &LAB_004319fa;
  puStack_20 = (undefined1 *)0x4319f2;
  System__LStrClr_40929B27(&local_8,uStack_24,puVar2);
  return;
}

