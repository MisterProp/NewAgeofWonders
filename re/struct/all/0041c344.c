
void FUN_0041c344(int param_1,undefined4 param_2)

{
  int *piVar1;
  undefined1 *puVar2;
  undefined4 uVar3;
  int iVar4;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_8;
  
  puStack_14 = &stack0xfffffffc;
  local_8 = 0;
  puStack_18 = &LAB_0041c740;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  System__LStrCat3_51F89FF7
            (&local_8,*(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x2c),"Int\\Scenes");
  SysUtils_SetCurrentDir_3C9BDBFB(local_8);
  uVar3 = FUN_0041bbb8(param_1);
  iVar4 = FUN_0040bde4(uVar3,param_2);
  uVar3 = *(undefined4 *)(iVar4 + 0x30);
  iVar4 = FUN_0041bbb8(param_1);
  iVar4 = FUN_0040bbb8(uVar3,*(undefined4 *)(iVar4 + 0x3c));
  AoWLabel_TAOWLabel_SetGText_23EDC2EF
            (*(undefined4 *)(param_1 + 0x78),*(undefined4 *)(iVar4 + 0x10));
  uVar3 = FUN_0041bbb8(param_1);
  iVar4 = FUN_0040bde4(uVar3,param_2);
  uVar3 = *(undefined4 *)(iVar4 + 0x30);
  iVar4 = FUN_0041bbb8(param_1);
  iVar4 = FUN_0040bbb8(uVar3,*(int *)(iVar4 + 0x3c) + 1);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF
            (*(undefined4 *)(param_1 + 0x80),*(undefined4 *)(iVar4 + 0x10));
  uVar3 = FUN_0041bbb8(param_1);
  iVar4 = FUN_0040bde4(uVar3,param_2);
  uVar3 = *(undefined4 *)(iVar4 + 0x30);
  iVar4 = FUN_0041bbb8(param_1);
  iVar4 = FUN_0040bbb8(uVar3,*(undefined4 *)(iVar4 + 0x3c));
  piVar1 = *(int **)(*(int *)(param_1 + 0x44) + 0x118);
  (**(code **)(*piVar1 + 8))(piVar1,*(undefined4 *)(iVar4 + 0x14));
  uVar3 = FUN_0041bbb8(param_1);
  iVar4 = FUN_0040bde4(uVar3,param_2);
  uVar3 = *(undefined4 *)(iVar4 + 0x30);
  iVar4 = FUN_0041bbb8(param_1);
  iVar4 = FUN_0040bbb8(uVar3,*(int *)(iVar4 + 0x3c) + 1);
  piVar1 = *(int **)(*(int *)(param_1 + 0x7c) + 0x118);
  (**(code **)(*piVar1 + 8))(piVar1,*(undefined4 *)(iVar4 + 0x14));
  AoWVScroll_TAOWScrollBar_SetFPos_23EDC2EF(*(undefined4 *)(param_1 + 0x134),0);
  AoWVScroll_TAOWScrollBar_SetFPos_23EDC2EF(*(undefined4 *)(param_1 + 0x138),0);
  uVar3 = FUN_0041bbb8(param_1);
  iVar4 = FUN_0040bde4(uVar3,param_2);
  uVar3 = *(undefined4 *)(iVar4 + 0x30);
  iVar4 = FUN_0041bbb8(param_1);
  iVar4 = FUN_0040bbb8(uVar3,*(undefined4 *)(iVar4 + 0x3c));
  if (*(int *)(iVar4 + 8) == 0) {
    AoWImage_TAOWImage_SetILB_23EDC2EF(*(undefined4 *)(param_1 + 0x84),0);
  }
  else {
    uVar3 = FUN_0041bbb8(param_1);
    iVar4 = FUN_0040bde4(uVar3,param_2);
    uVar3 = *(undefined4 *)(iVar4 + 0x30);
    iVar4 = FUN_0041bbb8(param_1);
    iVar4 = FUN_0040bbb8(uVar3,*(undefined4 *)(iVar4 + 0x3c));
    (**(code **)(**(int **)(*(int *)PTR_DAT_0045a4a0 + 0x8c) + 0x3c))
              (*(int **)(*(int *)PTR_DAT_0045a4a0 + 0x8c),*(undefined4 *)(iVar4 + 8));
    AoWImage_TAOWImage_SetILB_23EDC2EF
              (*(undefined4 *)(param_1 + 0x84),*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0x8c));
    uVar3 = FUN_0041bbb8(param_1);
    iVar4 = FUN_0040bde4(uVar3,param_2);
    uVar3 = *(undefined4 *)(iVar4 + 0x30);
    iVar4 = FUN_0041bbb8(param_1);
    iVar4 = FUN_0040bbb8(uVar3,*(undefined4 *)(iVar4 + 0x3c));
    AoWImage_TAOWImage_SetCurrent_23EDC2EF
              (*(undefined4 *)(param_1 + 0x84),*(undefined4 *)(iVar4 + 0xc));
    (**(code **)(**(int **)(param_1 + 0x84) + 0x60))(*(int **)(param_1 + 0x84),0);
    (**(code **)(**(int **)(param_1 + 0x84) + 100))(*(int **)(param_1 + 0x84),0);
    (**(code **)(**(int **)(param_1 + 0x84) + 0x6c))(*(int **)(param_1 + 0x84),1);
  }
  uVar3 = FUN_0041bbb8(param_1);
  iVar4 = FUN_0040bde4(uVar3,param_2);
  uVar3 = *(undefined4 *)(iVar4 + 0x30);
  iVar4 = FUN_0041bbb8(param_1);
  iVar4 = FUN_0040bbb8(uVar3,*(int *)(iVar4 + 0x3c) + 1);
  if (*(int *)(iVar4 + 8) == 0) {
    AoWImage_TAOWImage_SetILB_23EDC2EF(*(undefined4 *)(param_1 + 0x48),0);
  }
  else {
    uVar3 = FUN_0041bbb8(param_1);
    iVar4 = FUN_0040bde4(uVar3,param_2);
    uVar3 = *(undefined4 *)(iVar4 + 0x30);
    iVar4 = FUN_0041bbb8(param_1);
    iVar4 = FUN_0040bbb8(uVar3,*(int *)(iVar4 + 0x3c) + 1);
    (**(code **)(**(int **)(*(int *)PTR_DAT_0045a4a0 + 0x90) + 0x3c))
              (*(int **)(*(int *)PTR_DAT_0045a4a0 + 0x90),*(undefined4 *)(iVar4 + 8));
    AoWImage_TAOWImage_SetILB_23EDC2EF
              (*(undefined4 *)(param_1 + 0x48),*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0x90));
    uVar3 = FUN_0041bbb8(param_1);
    iVar4 = FUN_0040bde4(uVar3,param_2);
    uVar3 = *(undefined4 *)(iVar4 + 0x30);
    iVar4 = FUN_0041bbb8(param_1);
    iVar4 = FUN_0040bbb8(uVar3,*(int *)(iVar4 + 0x3c) + 1);
    AoWImage_TAOWImage_SetCurrent_23EDC2EF
              (*(undefined4 *)(param_1 + 0x48),*(undefined4 *)(iVar4 + 0xc));
    (**(code **)(**(int **)(param_1 + 0x48) + 0x60))(*(int **)(param_1 + 0x48),0);
    (**(code **)(**(int **)(param_1 + 0x48) + 100))(*(int **)(param_1 + 0x48),0);
    (**(code **)(**(int **)(param_1 + 0x48) + 0x6c))(*(int **)(param_1 + 0x48),1);
  }
  iVar4 = FUN_0041bbb8(param_1);
  (**(code **)(**(int **)(param_1 + 0x68) + 0x6c))
            (*(int **)(param_1 + 0x68),0 < *(int *)(iVar4 + 0x3c));
  iVar4 = FUN_0041bbb8(param_1);
  if (*(int *)(iVar4 + 0x3c) == 8) {
    (**(code **)(**(int **)(param_1 + 0x6c) + 0x6c))(*(int **)(param_1 + 0x6c),0);
  }
  else {
    uVar3 = FUN_0041bbb8(param_1);
    iVar4 = FUN_0040bde4(uVar3,param_2);
    uVar3 = *(undefined4 *)(iVar4 + 0x30);
    iVar4 = FUN_0041bbb8(param_1);
    iVar4 = FUN_0040bbb8(uVar3,*(int *)(iVar4 + 0x3c) + 2);
    if (*(int *)(iVar4 + 8) == 0) {
      uVar3 = FUN_0041bbb8(param_1);
      iVar4 = FUN_0040bde4(uVar3,param_2);
      uVar3 = *(undefined4 *)(iVar4 + 0x30);
      iVar4 = FUN_0041bbb8(param_1);
      iVar4 = FUN_0040bbb8(uVar3,*(int *)(iVar4 + 0x3c) + 2);
      iVar4 = (**(code **)(**(int **)(iVar4 + 0x14) + 0x14))();
      if (0 < iVar4) goto LAB_0041c69b;
      uVar3 = 0;
    }
    else {
LAB_0041c69b:
      uVar3 = 1;
    }
    (**(code **)(**(int **)(param_1 + 0x6c) + 0x6c))(*(int **)(param_1 + 0x6c),uVar3);
  }
  if (*(char *)(*(int *)(param_1 + 0x6c) + 0x75) != '\0') {
    iVar4 = FUN_0041bbb8(param_1);
    if (*(int *)(iVar4 + 0x38) != 10) {
      uVar3 = 0;
      goto LAB_0041c6c1;
    }
  }
  uVar3 = 1;
LAB_0041c6c1:
  (**(code **)(**(int **)(param_1 + 0x74) + 0x6c))(*(int **)(param_1 + 0x74),uVar3);
  (**(code **)(**(int **)(param_1 + 0x70) + 0x6c))
            (*(int **)(param_1 + 0x70),*(undefined1 *)(*(int *)(param_1 + 0x4c) + 0x75));
  piVar1 = *(int **)(param_1 + 0x88);
  *(undefined1 *)(piVar1 + 0x40) = 1;
  (**(code **)(*piVar1 + 0x6c))(piVar1,1);
  piVar1 = *(int **)(*(int *)(param_1 + 0x88) + 0x18c);
  if (piVar1 != (int *)0x0) {
    (**(code **)(*piVar1 + 0x44))(piVar1,*(undefined4 *)(*(int *)(param_1 + 0x88) + 0x184));
  }
  if (*(char *)(*(int *)(*(int *)PTR_DAT_0045a28c + 0x44) + 0x75) != '\0') {
    AOWWinManager_TAOWWinManager_SetOverPri_23EDC2EF
              (*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0x44),0xf9);
  }
  puVar2 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_0041c747;
  puStack_18 = (undefined1 *)0x41c73f;
  System__LStrClr_40929B27(&local_8,uStack_1c,puVar2);
  return;
}

