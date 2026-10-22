
void FUN_0040f828(int param_1)

{
  undefined1 *puVar1;
  int iVar2;
  undefined4 *in_FS_OFFSET;
  bool bVar3;
  undefined4 uStack_20;
  undefined1 *puStack_1c;
  undefined1 *puStack_18;
  int local_c;
  int local_8;
  
  puStack_18 = &stack0xfffffffc;
  local_c = 0;
  puStack_1c = &LAB_0040f8b8;
  uStack_20 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_20;
  local_8 = param_1;
  iVar2 = (**(code **)(**(int **)(param_1 + 0xe0) + 0x14))();
  if ((*(int *)(*(int *)(local_8 + 0x68) + 0x90) == 0) ||
     (*(int *)(*(int *)(local_8 + 0x9c) + 0x90) == 0)) {
    bVar3 = false;
  }
  else {
    bVar3 = true;
  }
  while ((puVar1 = puStack_18, bVar3 && (iVar2 != 0))) {
    iVar2 = iVar2 + -1;
    (**(code **)(**(int **)(local_8 + 0xe0) + 0xc))(*(int **)(local_8 + 0xe0),iVar2,&local_c);
    bVar3 = local_c != 0;
  }
  *in_FS_OFFSET = uStack_20;
  puStack_18 = &LAB_0040f8bf;
  puStack_1c = (undefined1 *)0x40f8b7;
  System__LStrClr_40929B27(&local_c,uStack_20,puVar1);
  return;
}

