
void FUN_00427200(int param_1,int param_2)

{
  int iVar1;
  int *piVar2;
  undefined1 *puVar3;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_20;
  undefined1 *puStack_1c;
  undefined1 *puStack_18;
  undefined4 local_8;
  
  puStack_18 = &stack0xfffffffc;
  local_8 = 0;
  puStack_1c = &LAB_00427253;
  uStack_20 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_20;
  iVar1 = *(int *)(param_1 + 0x27c);
  puVar3 = &stack0xfffffffc;
  if (iVar1 != 0) {
    (**(code **)(**(int **)(param_2 + 0x118) + 0x1c))(*(int **)(param_2 + 0x118),&local_8);
    piVar2 = *(int **)(iVar1 + 0x20);
    (**(code **)(*piVar2 + 0x2c))(piVar2,local_8);
    puVar3 = puStack_18;
  }
  puStack_18 = puVar3;
  puVar3 = puStack_18;
  *in_FS_OFFSET = uStack_20;
  puStack_18 = &LAB_0042725a;
  puStack_1c = (undefined1 *)0x427252;
  System__LStrClr_40929B27(&local_8,uStack_20,puVar3);
  return;
}

