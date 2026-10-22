
void FUN_0044275c(int param_1,int *param_2)

{
  undefined1 *puVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_8;
  
  puStack_14 = &stack0xfffffffc;
  local_8 = 0;
  puStack_18 = &LAB_004427f7;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  (**(code **)(*param_2 + 0x148))(param_2,&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 100),local_8);
  iVar2 = (**(code **)(*param_2 + 0x140))();
  if (iVar2 != 0) {
    uVar3 = (**(code **)(*param_2 + 0x138))();
    AoWArnoLine_TAoWArnoLine_SetITop_23EDC2EF(*(undefined4 *)(param_1 + 0x60),uVar3);
    uVar3 = (**(code **)(*param_2 + 0x13c))();
    AoWArnoLine_TAoWArnoLine_SetIMiddle_23EDC2EF(*(undefined4 *)(param_1 + 0x60),uVar3);
    uVar3 = (**(code **)(*param_2 + 0x140))();
    AoWArnoLine_TAoWArnoLine_SetIBottom_23EDC2EF(*(undefined4 *)(param_1 + 0x60),uVar3);
  }
  (**(code **)(**(int **)(param_1 + 0x60) + 0x6c))(*(int **)(param_1 + 0x60),1);
  puVar1 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_004427fe;
  puStack_18 = (undefined1 *)0x4427f6;
  System__LStrClr_40929B27(&local_8,uStack_1c,puVar1);
  return;
}

