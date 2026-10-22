
void FUN_00437b3c(int param_1)

{
  int *piVar1;
  undefined1 *puVar2;
  char cVar3;
  undefined4 uVar4;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_8;
  
  puStack_14 = &stack0xfffffffc;
  local_8 = 0;
  puStack_18 = &LAB_00437bdc;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  cVar3 = FUN_004379f0(param_1);
  if (cVar3 != '\0') {
    (**(code **)(**(int **)(param_1 + 0xb4) + 0x30))();
    uVar4 = AoWE_TUnitControl_FindUnit_23EDC2EF
                      (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xfc),
                       *(undefined4 *)(*(int *)(param_1 + 0xac) + 0x18));
    *(undefined4 *)(param_1 + 0xb8) = uVar4;
    (**(code **)(**(int **)(param_1 + 0xac) + 0x7c))
              (*(int **)(param_1 + 0xac),*(undefined4 *)(param_1 + 0xb4),param_1 + 0xb1);
    piVar1 = *(int **)(*(int *)(param_1 + 0xac) + 0x2c);
    (**(code **)(*piVar1 + 0x1c))(piVar1,&local_8);
    piVar1 = *(int **)(*(int *)(param_1 + 0x8c) + 0x118);
    (**(code **)(*piVar1 + 0x2c))(piVar1,local_8);
  }
  puVar2 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_00437be3;
  puStack_18 = (undefined1 *)0x437bdb;
  System__LStrClr_40929B27(&local_8,uStack_1c,puVar2);
  return;
}

