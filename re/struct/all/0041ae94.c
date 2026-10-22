
void FUN_0041ae94(int param_1,undefined4 param_2,undefined1 param_3,undefined4 param_4,
                 undefined4 param_5)

{
  int iVar1;
  undefined1 *puVar2;
  int iVar3;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_8;
  
  puStack_14 = &stack0xfffffffc;
  local_8 = 0;
  puStack_18 = &LAB_0041b050;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  *(undefined4 *)(param_1 + 0x70) = param_5;
  *(undefined4 *)(param_1 + 0x74) = param_4;
  *(undefined1 *)(param_1 + 0x6c) = param_3;
  iVar3 = FUN_00419488(param_1);
  iVar1 = *(int *)(param_1 + 0x7c);
  if (iVar3 == iVar1) {
    *(int *)(param_1 + 0x78) = iVar1;
    switch(iVar1) {
    case 0:
      (**(code **)(**(int **)(param_1 + 0x48) + 0x6c))(*(int **)(param_1 + 0x48),0);
      FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,0);
      FUN_0042abf0(*(undefined4 *)PTR_DAT_0045a598,0);
      break;
    case 1:
      if (*(char *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x6e) == '\0') {
        (**(code **)(**(int **)(param_1 + 0x48) + 0x6c))(*(int **)(param_1 + 0x48),0);
        FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,0);
        FUN_00419158(*(undefined4 *)PTR_DAT_0045a230);
      }
      else {
        FUN_0040eac4();
      }
      break;
    case 2:
      System__LStrCatN_51F89FF7(&local_8,3);
      FUN_00454b70(*(undefined4 *)PTR_DAT_0045a420,local_8);
      if (*(int *)AoWE_AoWHSMap_6B957A58_exref != 0) {
        *(undefined1 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x148) = 1;
        FUN_00454d2c(*(undefined4 *)PTR_DAT_0045a420);
        (**(code **)(**(int **)(param_1 + 0x48) + 0x6c))(*(int **)(param_1 + 0x48),0);
      }
      break;
    case 3:
      FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,0);
      *(undefined4 *)(*(int *)PTR_DAT_0045a4c8 + 0x148) = 0;
      (**(code **)(**(int **)(*(int *)PTR_DAT_0045a4c8 + 0x44) + 0x6c))
                (*(int **)(*(int *)PTR_DAT_0045a4c8 + 0x44),1);
      break;
    case 4:
      (**(code **)(**(int **)(param_1 + 0x48) + 0x6c))(*(int **)(param_1 + 0x48),0);
      FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,0);
      if (*(char *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x6e) == '\0') {
        Forms_TCustomForm_Close_23EDC2EF(*(undefined4 *)PTR_DAT_0045a168);
      }
      else {
        FUN_00429c14(*(undefined4 *)PTR_DAT_0045a28c);
      }
    }
  }
  puVar2 = puStack_14;
  *(undefined4 *)(param_1 + 0x7c) = 0xffffffff;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_0041b057;
  puStack_18 = (undefined1 *)0x41b04f;
  System__LStrClr_40929B27(&local_8,uStack_1c,puVar2);
  return;
}

