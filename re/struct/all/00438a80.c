
void FUN_00438a80(int param_1)

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
  puStack_14 = &LAB_00438af8;
  uStack_18 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_18;
  cVar2 = FUN_00438628(param_1);
  if (cVar2 != '\0') {
    FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,0);
    cVar2 = (**(code **)(**(int **)(param_1 + 0x9c) + 0x88))(*(int **)(param_1 + 0x9c),&local_8);
    if (cVar2 == '\0') {
      (**(code **)(**(int **)AoWE_AoWHSMap_6B957A58_exref + 0x114))
                (*(int **)AoWE_AoWHSMap_6B957A58_exref,local_8);
    }
    else {
      (**(code **)(**(int **)(param_1 + 0x78) + 0x6c))(*(int **)(param_1 + 0x78),0);
    }
  }
  puVar1 = puStack_10;
  *in_FS_OFFSET = uStack_18;
  puStack_10 = &LAB_00438aff;
  puStack_14 = (undefined1 *)0x438af7;
  System__LStrClr_40929B27(&local_8,uStack_18,puVar1);
  return;
}

