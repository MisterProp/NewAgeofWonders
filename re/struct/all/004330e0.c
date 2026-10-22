
void FUN_004330e0(int param_1)

{
  undefined1 *puVar1;
  char cVar2;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_18;
  undefined1 *puStack_14;
  undefined1 *puStack_10;
  undefined4 local_8;
  
  puStack_10 = &stack0xfffffffc;
  local_8 = 0;
  puStack_14 = &LAB_0043314b;
  uStack_18 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_18;
  cVar2 = (**(code **)(**(int **)(param_1 + 0xa4) + 0x4c))(*(int **)(param_1 + 0xa4),&local_8);
  if (cVar2 == '\0') {
    (**(code **)(**(int **)AoWE_AoWHSMap_6B957A58_exref + 0x110))
              (*(int **)AoWE_AoWHSMap_6B957A58_exref,local_8);
  }
  else {
    cVar2 = (**(code **)(**(int **)(param_1 + 0xa4) + 0x50))();
    if (cVar2 != '\0') {
      (**(code **)(**(int **)(param_1 + 0x58) + 0x6c))(*(int **)(param_1 + 0x58),0);
    }
  }
  puVar1 = puStack_10;
  *in_FS_OFFSET = uStack_18;
  puStack_10 = &LAB_00433152;
  puStack_14 = (undefined1 *)0x43314a;
  System__LStrClr_40929B27(&local_8,uStack_18,puVar1);
  return;
}

