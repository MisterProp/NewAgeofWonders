
void FUN_00442278(void)

{
  int *piVar1;
  undefined1 *puVar2;
  char cVar3;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_18;
  undefined1 *puStack_14;
  undefined1 *puStack_10;
  undefined4 local_8;
  
  puStack_10 = &stack0xfffffffc;
  local_8 = 0;
  puStack_14 = &LAB_0044231a;
  uStack_18 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_18;
  FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,0);
  piVar1 = *(int **)(*(int *)PTR_DAT_0045a420 + 0xa8);
  if (piVar1 != (int *)0x0) {
    cVar3 = (**(code **)(*piVar1 + 0x1ec))
                      (piVar1,*(undefined1 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5),&local_8)
    ;
    if (cVar3 == '\0') {
      (**(code **)(**(int **)AoWE_AoWHSMap_6B957A58_exref + 0x114))
                (*(int **)AoWE_AoWHSMap_6B957A58_exref,local_8);
    }
    else {
      (**(code **)(**(int **)(*(int *)PTR_DAT_0045a420 + 0xa8) + 0x1e8))
                (*(int **)(*(int *)PTR_DAT_0045a420 + 0xa8),
                 *(undefined1 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
    }
  }
  puVar2 = puStack_10;
  *in_FS_OFFSET = uStack_18;
  puStack_10 = &LAB_00442321;
  puStack_14 = (undefined1 *)0x442319;
  System__LStrClr_40929B27(&local_8,uStack_18,puVar2);
  return;
}

