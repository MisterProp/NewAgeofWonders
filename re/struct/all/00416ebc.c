
void FUN_00416ebc(int param_1)

{
  int iVar1;
  int *piVar2;
  undefined1 *puVar3;
  uint uVar4;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_14 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  puStack_18 = &LAB_0041704b;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,0);
  iVar1 = *(int *)(param_1 + 0x2dc);
  if (iVar1 == 1) {
    FUN_00415a94(param_1);
    if ((*(byte *)(*(int *)(param_1 + 0x2d8) + 0x10) & 2) == 0) {
      if ((*(byte *)(*(int *)(param_1 + 0x2d8) + 0x10) & 4) == 0) {
        FUN_00416d9c(param_1);
      }
      else {
        FUN_00416650(param_1,3);
      }
    }
    else {
      FUN_00416650(param_1,2);
    }
    *(undefined1 *)(*(int *)(*(int *)PTR_DAT_0045a28c + 0x44) + 0xa4) = 0;
  }
  else if (iVar1 == 2) {
    FUN_00415a94(param_1);
    if ((*(byte *)(*(int *)(param_1 + 0x2d8) + 0x10) & 4) == 0) {
      FUN_00416d9c(param_1);
    }
    else {
      FUN_00416650(param_1,3);
    }
    *(undefined1 *)(*(int *)(*(int *)PTR_DAT_0045a28c + 0x44) + 0xa4) = 0;
  }
  else if (iVar1 == 3) {
    uVar4 = AoWE_TLeader_GetSphereCount_23EDC2EF(*(undefined4 *)(param_1 + 0x2e4));
    if (uVar4 == *(byte *)(*(int *)(param_1 + 0x2e4) + 0x9c)) {
      FUN_00416d9c(param_1);
    }
    else {
      FUN_0040e7e4(*(undefined4 *)PTR_DAT_0045a1a0);
      piVar2 = *(int **)(*(int *)(*(int *)PTR_DAT_0045a1a0 + 0x80) + 0x118);
      (**(code **)(*piVar2 + 0x34))(piVar2,0);
      System_LoadResString_02614E1B(AoWE_PleaseAssignAllSpheresRStr_04ECFCC5_exref,&local_c);
      AoWE_TranslateRStr_435799AA(local_c,&local_8);
      piVar2 = *(int **)(*(int *)(*(int *)PTR_DAT_0045a1a0 + 0x80) + 0x118);
      (**(code **)(*piVar2 + 0x34))(piVar2,local_8);
      (**(code **)(**(int **)(*(int *)PTR_DAT_0045a1a0 + 0x70) + 0x6c))
                (*(int **)(*(int *)PTR_DAT_0045a1a0 + 0x70),1);
      (**(code **)(**(int **)(*(int *)PTR_DAT_0045a1a0 + 0x44) + 0x6c))
                (*(int **)(*(int *)PTR_DAT_0045a1a0 + 0x44),1);
    }
  }
  puVar3 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_00417052;
  puStack_18 = (undefined1 *)0x41704a;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar3);
  return;
}

