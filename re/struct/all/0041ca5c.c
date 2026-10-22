
void FUN_0041ca5c(int param_1)

{
  char cVar1;
  undefined1 *puVar2;
  int iVar3;
  undefined4 uVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  undefined4 uVar8;
  int *piVar9;
  undefined4 extraout_EDX;
  undefined4 extraout_EDX_00;
  undefined4 *in_FS_OFFSET;
  char *pcVar10;
  undefined4 uVar11;
  undefined4 uStack_20;
  undefined1 *puStack_1c;
  undefined1 *puStack_18;
  undefined4 local_8;
  
  puStack_18 = &stack0xfffffffc;
  local_8 = 0;
  puStack_1c = &LAB_0041d77e;
  uStack_20 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_20;
  iVar3 = FUN_0041bbb8(param_1);
  if (iVar3 == 0) goto switchD_0041ca96_caseD_5;
  iVar3 = FUN_0041bbb8(param_1);
  switch(*(undefined4 *)(iVar3 + 0x38)) {
  case 0:
    iVar3 = FUN_0041bbb8(param_1);
    uVar4 = *(undefined4 *)(iVar3 + 0x24);
    uVar8 = FUN_0041bbb8(param_1);
    iVar3 = FUN_0040bde4(uVar8,uVar4);
    iVar3 = FUN_0040bbb8(*(undefined4 *)(iVar3 + 0x34),0);
    if (*(int *)(iVar3 + 8) == 0) {
      iVar3 = FUN_0041bbb8(param_1);
      uVar4 = *(undefined4 *)(iVar3 + 0x24);
      uVar8 = FUN_0041bbb8(param_1);
      iVar3 = FUN_0040bde4(uVar8,uVar4);
      iVar3 = FUN_0040bbb8(*(undefined4 *)(iVar3 + 0x34),0);
      iVar3 = (**(code **)(**(int **)(iVar3 + 0x14) + 0x14))();
      if (iVar3 < 1) {
        iVar3 = FUN_0041bbb8(param_1);
        *(int *)(iVar3 + 0x38) = *(int *)(iVar3 + 0x38) + 1;
        FUN_0041ca5c(param_1);
        break;
      }
    }
    System__LStrCat3_51F89FF7
              (&local_8,*(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x2c),
               "\\Int\\Scenes");
    SysUtils_SetCurrentDir_3C9BDBFB(local_8);
    iVar3 = FUN_0041bbb8(param_1);
    uVar4 = *(undefined4 *)(iVar3 + 0x24);
    uVar8 = FUN_0041bbb8(param_1);
    iVar3 = FUN_0040bde4(uVar8,uVar4);
    uVar4 = *(undefined4 *)(iVar3 + 0x34);
    iVar3 = FUN_0041bbb8(param_1);
    iVar3 = FUN_0040bbb8(uVar4,*(undefined4 *)(iVar3 + 0x3c));
    AoWLabel_TAOWLabel_SetGText_23EDC2EF
              (*(undefined4 *)(param_1 + 0x78),*(undefined4 *)(iVar3 + 0x10));
    iVar3 = FUN_0041bbb8(param_1);
    uVar4 = *(undefined4 *)(iVar3 + 0x24);
    uVar8 = FUN_0041bbb8(param_1);
    iVar3 = FUN_0040bde4(uVar8,uVar4);
    uVar4 = *(undefined4 *)(iVar3 + 0x34);
    iVar3 = FUN_0041bbb8(param_1);
    iVar3 = FUN_0040bbb8(uVar4,*(int *)(iVar3 + 0x3c) + 1);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF
              (*(undefined4 *)(param_1 + 0x80),*(undefined4 *)(iVar3 + 0x10));
    iVar3 = FUN_0041bbb8(param_1);
    uVar4 = *(undefined4 *)(iVar3 + 0x24);
    uVar8 = FUN_0041bbb8(param_1);
    iVar3 = FUN_0040bde4(uVar8,uVar4);
    uVar4 = *(undefined4 *)(iVar3 + 0x34);
    iVar3 = FUN_0041bbb8(param_1);
    iVar3 = FUN_0040bbb8(uVar4,*(undefined4 *)(iVar3 + 0x3c));
    piVar9 = *(int **)(*(int *)(param_1 + 0x44) + 0x118);
    (**(code **)(*piVar9 + 8))(piVar9,*(undefined4 *)(iVar3 + 0x14));
    iVar3 = FUN_0041bbb8(param_1);
    uVar4 = *(undefined4 *)(iVar3 + 0x24);
    uVar8 = FUN_0041bbb8(param_1);
    iVar3 = FUN_0040bde4(uVar8,uVar4);
    uVar4 = *(undefined4 *)(iVar3 + 0x34);
    iVar3 = FUN_0041bbb8(param_1);
    iVar3 = FUN_0040bbb8(uVar4,*(int *)(iVar3 + 0x3c) + 1);
    piVar9 = *(int **)(*(int *)(param_1 + 0x7c) + 0x118);
    (**(code **)(*piVar9 + 8))(piVar9,*(undefined4 *)(iVar3 + 0x14));
    *(undefined4 *)(param_1 + 0x144) = 0x32;
    *(undefined4 *)(param_1 + 0x140) = 1;
    iVar3 = FUN_0041bbb8(param_1);
    uVar4 = *(undefined4 *)(iVar3 + 0x24);
    uVar8 = FUN_0041bbb8(param_1);
    iVar3 = FUN_0040bde4(uVar8,uVar4);
    cVar1 = *(char *)(iVar3 + 0x38);
    if (cVar1 == '\0') {
      AoWBaseWin_TAOWBaseWindow_SetILTIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x4c),0);
      *(undefined4 *)(param_1 + 0x13c) = 0xd;
    }
    else if (cVar1 == '\x01') {
      AoWBaseWin_TAOWBaseWindow_SetILTIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x4c),10);
      *(undefined4 *)(param_1 + 0x13c) = 0xe;
    }
    else if (cVar1 == '\x02') {
      AoWBaseWin_TAOWBaseWindow_SetILTIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x4c),0xb);
      *(undefined4 *)(param_1 + 0x13c) = 0xf;
    }
    iVar3 = FUN_0041bbb8(param_1);
    uVar4 = *(undefined4 *)(iVar3 + 0x24);
    uVar8 = FUN_0041bbb8(param_1);
    iVar3 = FUN_0040bde4(uVar8,uVar4);
    uVar4 = *(undefined4 *)(iVar3 + 0x34);
    iVar3 = FUN_0041bbb8(param_1);
    iVar3 = FUN_0040bbb8(uVar4,*(undefined4 *)(iVar3 + 0x3c));
    if (*(int *)(iVar3 + 8) == 0) {
      AoWImage_TAOWImage_SetILB_23EDC2EF(*(undefined4 *)(param_1 + 0x84),0);
    }
    else {
      iVar3 = FUN_0041bbb8(param_1);
      uVar4 = *(undefined4 *)(iVar3 + 0x24);
      uVar8 = FUN_0041bbb8(param_1);
      iVar3 = FUN_0040bde4(uVar8,uVar4);
      uVar4 = *(undefined4 *)(iVar3 + 0x34);
      iVar3 = FUN_0041bbb8(param_1);
      iVar3 = FUN_0040bbb8(uVar4,*(undefined4 *)(iVar3 + 0x3c));
      (**(code **)(**(int **)(*(int *)PTR_DAT_0045a4a0 + 0x8c) + 0x3c))
                (*(int **)(*(int *)PTR_DAT_0045a4a0 + 0x8c),*(undefined4 *)(iVar3 + 8));
      AoWImage_TAOWImage_SetILB_23EDC2EF
                (*(undefined4 *)(param_1 + 0x84),*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0x8c));
      iVar3 = FUN_0041bbb8(param_1);
      uVar4 = *(undefined4 *)(iVar3 + 0x24);
      uVar8 = FUN_0041bbb8(param_1);
      iVar3 = FUN_0040bde4(uVar8,uVar4);
      uVar4 = *(undefined4 *)(iVar3 + 0x34);
      iVar3 = FUN_0041bbb8(param_1);
      iVar3 = FUN_0040bbb8(uVar4,*(undefined4 *)(iVar3 + 0x3c));
      AoWImage_TAOWImage_SetCurrent_23EDC2EF
                (*(undefined4 *)(param_1 + 0x84),*(undefined4 *)(iVar3 + 0xc));
      (**(code **)(**(int **)(param_1 + 0x84) + 0x60))(*(int **)(param_1 + 0x84),0);
      (**(code **)(**(int **)(param_1 + 0x84) + 100))(*(int **)(param_1 + 0x84),0);
      (**(code **)(**(int **)(param_1 + 0x84) + 0x6c))(*(int **)(param_1 + 0x84),1);
    }
    iVar3 = FUN_0041bbb8(param_1);
    uVar4 = *(undefined4 *)(iVar3 + 0x24);
    uVar8 = FUN_0041bbb8(param_1);
    iVar3 = FUN_0040bde4(uVar8,uVar4);
    uVar4 = *(undefined4 *)(iVar3 + 0x34);
    iVar3 = FUN_0041bbb8(param_1);
    iVar3 = FUN_0040bbb8(uVar4,*(int *)(iVar3 + 0x3c) + 1);
    if (*(int *)(iVar3 + 8) == 0) {
      AoWImage_TAOWImage_SetILB_23EDC2EF(*(undefined4 *)(param_1 + 0x48),0);
    }
    else {
      iVar3 = FUN_0041bbb8(param_1);
      uVar4 = *(undefined4 *)(iVar3 + 0x24);
      uVar8 = FUN_0041bbb8(param_1);
      iVar3 = FUN_0040bde4(uVar8,uVar4);
      uVar4 = *(undefined4 *)(iVar3 + 0x34);
      iVar3 = FUN_0041bbb8(param_1);
      iVar3 = FUN_0040bbb8(uVar4,*(int *)(iVar3 + 0x3c) + 1);
      (**(code **)(**(int **)(*(int *)PTR_DAT_0045a4a0 + 0x90) + 0x3c))
                (*(int **)(*(int *)PTR_DAT_0045a4a0 + 0x90),*(undefined4 *)(iVar3 + 8));
      AoWImage_TAOWImage_SetILB_23EDC2EF
                (*(undefined4 *)(param_1 + 0x48),*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0x90));
      iVar3 = FUN_0041bbb8(param_1);
      uVar4 = *(undefined4 *)(iVar3 + 0x24);
      uVar8 = FUN_0041bbb8(param_1);
      iVar3 = FUN_0040bde4(uVar8,uVar4);
      uVar4 = *(undefined4 *)(iVar3 + 0x34);
      iVar3 = FUN_0041bbb8(param_1);
      iVar3 = FUN_0040bbb8(uVar4,*(int *)(iVar3 + 0x3c) + 1);
      AoWImage_TAOWImage_SetCurrent_23EDC2EF
                (*(undefined4 *)(param_1 + 0x48),*(undefined4 *)(iVar3 + 0xc));
      (**(code **)(**(int **)(param_1 + 0x48) + 0x60))(*(int **)(param_1 + 0x48),0);
      (**(code **)(**(int **)(param_1 + 0x48) + 100))(*(int **)(param_1 + 0x48),0);
      (**(code **)(**(int **)(param_1 + 0x48) + 0x6c))(*(int **)(param_1 + 0x48),1);
    }
    iVar3 = FUN_0041bbb8(param_1);
    (**(code **)(**(int **)(param_1 + 0x68) + 0x6c))
              (*(int **)(param_1 + 0x68),0 < *(int *)(iVar3 + 0x3c));
    iVar3 = FUN_0041bbb8(param_1);
    if (*(int *)(iVar3 + 0x3c) == 8) {
      (**(code **)(**(int **)(param_1 + 0x6c) + 0x6c))(*(int **)(param_1 + 0x6c),0);
      uVar4 = extraout_EDX;
    }
    else {
      iVar3 = FUN_0041bbb8(param_1);
      uVar4 = *(undefined4 *)(iVar3 + 0x24);
      uVar8 = FUN_0041bbb8(param_1);
      iVar3 = FUN_0040bde4(uVar8,uVar4);
      uVar4 = *(undefined4 *)(iVar3 + 0x34);
      iVar3 = FUN_0041bbb8(param_1);
      iVar3 = FUN_0040bbb8(uVar4,*(int *)(iVar3 + 0x3c) + 2);
      if (*(int *)(iVar3 + 8) == 0) {
        iVar3 = FUN_0041bbb8(param_1);
        uVar4 = *(undefined4 *)(iVar3 + 0x24);
        uVar8 = FUN_0041bbb8(param_1);
        iVar3 = FUN_0040bde4(uVar8,uVar4);
        uVar4 = *(undefined4 *)(iVar3 + 0x34);
        iVar3 = FUN_0041bbb8(param_1);
        iVar3 = FUN_0040bbb8(uVar4,*(int *)(iVar3 + 0x3c) + 2);
        iVar3 = (**(code **)(**(int **)(iVar3 + 0x14) + 0x14))();
        if (0 < iVar3) goto LAB_0041cf40;
        uVar4 = 0;
      }
      else {
LAB_0041cf40:
        uVar4 = 1;
      }
      (**(code **)(**(int **)(param_1 + 0x6c) + 0x6c))(*(int **)(param_1 + 0x6c),uVar4);
      uVar4 = extraout_EDX_00;
    }
    (**(code **)(**(int **)(param_1 + 0x74) + 0x6c))
              (*(int **)(param_1 + 0x74),
               CONCAT31((int3)((uint)uVar4 >> 8),*(undefined1 *)(*(int *)(param_1 + 0x6c) + 0x75)) ^
               1);
    (**(code **)(**(int **)(param_1 + 0x70) + 0x6c))(*(int **)(param_1 + 0x70),0);
    piVar9 = *(int **)(param_1 + 0x88);
    *(undefined1 *)(piVar9 + 0x40) = 1;
    (**(code **)(*piVar9 + 0x6c))(piVar9,1);
    (**(code **)(**(int **)(param_1 + 300) + 0x6c))(*(int **)(param_1 + 300),0);
    (**(code **)(**(int **)(param_1 + 0x100) + 0x6c))(*(int **)(param_1 + 0x100),0);
    (**(code **)(**(int **)(param_1 + 0xfc) + 0x6c))(*(int **)(param_1 + 0xfc),0);
    piVar9 = *(int **)(*(int *)(param_1 + 0x88) + 0x18c);
    if (piVar9 != (int *)0x0) {
      (**(code **)(*piVar9 + 0x44))(piVar9,*(undefined4 *)(*(int *)(param_1 + 0x88) + 0x184));
    }
    if (*(char *)(*(int *)(*(int *)PTR_DAT_0045a28c + 0x44) + 0x75) != '\0') {
      AOWWinManager_TAOWWinManager_SetOverPri_23EDC2EF
                (*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0x44),0xf9);
    }
    break;
  case 1:
    (**(code **)(**(int **)(param_1 + 300) + 0x6c))(*(int **)(param_1 + 300),1);
    (**(code **)(**(int **)(param_1 + 0x100) + 0x6c))(*(int **)(param_1 + 0x100),1);
    (**(code **)(**(int **)(param_1 + 0xfc) + 0x6c))(*(int **)(param_1 + 0xfc),1);
    (**(code **)(**(int **)(param_1 + 0x88) + 0x6c))(*(int **)(param_1 + 0x88),0);
    (**(code **)(**(int **)(param_1 + 0x5c) + 0x6c))(*(int **)(param_1 + 0x5c),1);
    (**(code **)(**(int **)(param_1 + 0x60) + 0x6c))(*(int **)(param_1 + 0x60),1);
    iVar3 = FUN_0041bbb8(param_1);
    uVar4 = *(undefined4 *)(iVar3 + 0x24);
    uVar8 = FUN_0041bbb8(param_1);
    iVar3 = FUN_0040bde4(uVar8,uVar4);
    (**(code **)(**(int **)(param_1 + 0x5c) + 0x58))
              (*(int **)(param_1 + 0x5c),*(int *)(iVar3 + 0x24) + -0x19);
    iVar3 = FUN_0041bbb8(param_1);
    uVar4 = *(undefined4 *)(iVar3 + 0x24);
    uVar8 = FUN_0041bbb8(param_1);
    iVar3 = FUN_0040bde4(uVar8,uVar4);
    (**(code **)(**(int **)(param_1 + 0x5c) + 0x5c))
              (*(int **)(param_1 + 0x5c),*(int *)(iVar3 + 0x28) + -0x41);
    iVar3 = FUN_0041bbb8(param_1);
    uVar4 = *(undefined4 *)(iVar3 + 0x24);
    uVar8 = FUN_0041bbb8(param_1);
    iVar3 = FUN_0040bde4(uVar8,uVar4);
    uVar4 = Engine_TIntegerList_Get_23EDC2EF(*(undefined4 *)(iVar3 + 0x2c),3);
    iVar3 = FUN_0041bbb8(param_1);
    iVar3 = AoWE_TAoWCampaignScenarioList_IndexOfID_23EDC2EF(*(undefined4 *)(iVar3 + 0x10),uVar4);
    if (iVar3 != -1) {
      uVar4 = FUN_0041bbb8(param_1);
      iVar5 = FUN_0040bde4(uVar4,iVar3);
      (**(code **)(**(int **)(param_1 + 0x60) + 0x58))
                (*(int **)(param_1 + 0x60),*(int *)(iVar5 + 0x24) + -0x19);
      uVar4 = FUN_0041bbb8(param_1);
      iVar3 = FUN_0040bde4(uVar4,iVar3);
      (**(code **)(**(int **)(param_1 + 0x60) + 0x5c))
                (*(int **)(param_1 + 0x60),*(int *)(iVar3 + 0x28) + -0x41);
    }
    piVar9 = *(int **)(param_1 + 0x5c);
    if (*(int *)(*(int *)(param_1 + 0x60) + 0x88) < piVar9[0x22]) {
      (**(code **)(*piVar9 + 0x68))(piVar9,10);
      (**(code **)(**(int **)(param_1 + 0x60) + 0x68))(*(int **)(param_1 + 0x60),5);
    }
    else {
      (**(code **)(*piVar9 + 0x68))(piVar9,5);
      (**(code **)(**(int **)(param_1 + 0x60) + 0x68))(*(int **)(param_1 + 0x60),10);
    }
    iVar3 = FUN_0041bbb8(param_1);
    uVar4 = *(undefined4 *)(iVar3 + 0x24);
    uVar8 = FUN_0041bbb8(param_1);
    iVar3 = FUN_0040bde4(uVar8,uVar4);
    iVar3 = Engine_TIntegerList_Get_23EDC2EF(*(undefined4 *)(iVar3 + 0x2c),0);
    if (iVar3 < 0) {
      (**(code **)(**(int **)(param_1 + 0x50) + 0x6c))(*(int **)(param_1 + 0x50),0);
    }
    else {
      (**(code **)(**(int **)(param_1 + 0x50) + 0x6c))(*(int **)(param_1 + 0x50),1);
      iVar3 = FUN_0041bbb8(param_1);
      AoWImage_TAOWImage_SetCurrent_23EDC2EF
                (*(undefined4 *)(param_1 + 0x50),
                 *(int *)(iVar3 + 0x34) * 3 + *(int *)(param_1 + 0x148) + 2);
      iVar3 = FUN_0041bbb8(param_1);
      uVar4 = *(undefined4 *)(iVar3 + 0x24);
      uVar8 = FUN_0041bbb8(param_1);
      iVar3 = FUN_0040bde4(uVar8,uVar4);
      uVar4 = Engine_TIntegerList_Get_23EDC2EF(*(undefined4 *)(iVar3 + 0x2c),0);
      iVar3 = FUN_0041bbb8(param_1);
      uVar4 = AoWE_TAoWCampaignScenarioList_IndexOfID_23EDC2EF(*(undefined4 *)(iVar3 + 0x10),uVar4);
      uVar8 = FUN_0041bbb8(param_1);
      iVar3 = FUN_0040bde4(uVar8,uVar4);
      (**(code **)(**(int **)(param_1 + 0x50) + 0x58))
                (*(int **)(param_1 + 0x50),*(undefined4 *)(iVar3 + 0x24));
      uVar8 = FUN_0041bbb8(param_1);
      iVar3 = FUN_0040bde4(uVar8,uVar4);
      (**(code **)(**(int **)(param_1 + 0x50) + 0x5c))
                (*(int **)(param_1 + 0x50),*(undefined4 *)(iVar3 + 0x28));
    }
    iVar3 = FUN_0041bbb8(param_1);
    uVar4 = *(undefined4 *)(iVar3 + 0x24);
    uVar8 = FUN_0041bbb8(param_1);
    iVar3 = FUN_0040bde4(uVar8,uVar4);
    iVar3 = Engine_TIntegerList_Get_23EDC2EF(*(undefined4 *)(iVar3 + 0x2c),1);
    if (iVar3 < 0) {
      (**(code **)(**(int **)(param_1 + 0x54) + 0x6c))(*(int **)(param_1 + 0x54),0);
    }
    else {
      (**(code **)(**(int **)(param_1 + 0x54) + 0x6c))(*(int **)(param_1 + 0x54),1);
      iVar3 = FUN_0041bbb8(param_1);
      AoWImage_TAOWImage_SetCurrent_23EDC2EF
                (*(undefined4 *)(param_1 + 0x54),
                 *(int *)(iVar3 + 0x34) * 3 + *(int *)(param_1 + 0x148) + 2);
      iVar3 = FUN_0041bbb8(param_1);
      uVar4 = *(undefined4 *)(iVar3 + 0x24);
      uVar8 = FUN_0041bbb8(param_1);
      iVar3 = FUN_0040bde4(uVar8,uVar4);
      uVar4 = Engine_TIntegerList_Get_23EDC2EF(*(undefined4 *)(iVar3 + 0x2c),1);
      iVar3 = FUN_0041bbb8(param_1);
      uVar4 = AoWE_TAoWCampaignScenarioList_IndexOfID_23EDC2EF(*(undefined4 *)(iVar3 + 0x10),uVar4);
      uVar8 = FUN_0041bbb8(param_1);
      iVar3 = FUN_0040bde4(uVar8,uVar4);
      (**(code **)(**(int **)(param_1 + 0x54) + 0x58))
                (*(int **)(param_1 + 0x54),*(undefined4 *)(iVar3 + 0x24));
      uVar8 = FUN_0041bbb8(param_1);
      iVar3 = FUN_0040bde4(uVar8,uVar4);
      (**(code **)(**(int **)(param_1 + 0x54) + 0x5c))
                (*(int **)(param_1 + 0x54),*(undefined4 *)(iVar3 + 0x28));
    }
    iVar3 = FUN_0041bbb8(param_1);
    uVar4 = *(undefined4 *)(iVar3 + 0x24);
    uVar8 = FUN_0041bbb8(param_1);
    iVar3 = FUN_0040bde4(uVar8,uVar4);
    iVar3 = Engine_TIntegerList_Get_23EDC2EF(*(undefined4 *)(iVar3 + 0x2c),2);
    if (iVar3 < 0) {
      (**(code **)(**(int **)(param_1 + 0x58) + 0x6c))(*(int **)(param_1 + 0x58),0);
    }
    else {
      (**(code **)(**(int **)(param_1 + 0x58) + 0x6c))(*(int **)(param_1 + 0x58),1);
      iVar3 = FUN_0041bbb8(param_1);
      AoWImage_TAOWImage_SetCurrent_23EDC2EF
                (*(undefined4 *)(param_1 + 0x58),
                 *(int *)(iVar3 + 0x34) * 3 + *(int *)(param_1 + 0x148) + 2);
      iVar3 = FUN_0041bbb8(param_1);
      uVar4 = *(undefined4 *)(iVar3 + 0x24);
      uVar8 = FUN_0041bbb8(param_1);
      iVar3 = FUN_0040bde4(uVar8,uVar4);
      uVar4 = Engine_TIntegerList_Get_23EDC2EF(*(undefined4 *)(iVar3 + 0x2c),2);
      iVar3 = FUN_0041bbb8(param_1);
      uVar4 = AoWE_TAoWCampaignScenarioList_IndexOfID_23EDC2EF(*(undefined4 *)(iVar3 + 0x10),uVar4);
      uVar8 = FUN_0041bbb8(param_1);
      iVar3 = FUN_0040bde4(uVar8,uVar4);
      (**(code **)(**(int **)(param_1 + 0x58) + 0x58))
                (*(int **)(param_1 + 0x58),*(undefined4 *)(iVar3 + 0x24));
      uVar8 = FUN_0041bbb8(param_1);
      iVar3 = FUN_0040bde4(uVar8,uVar4);
      (**(code **)(**(int **)(param_1 + 0x58) + 0x5c))
                (*(int **)(param_1 + 0x58),*(undefined4 *)(iVar3 + 0x28));
    }
    if (((*(char *)(*(int *)(param_1 + 0x50) + 0x75) == '\0') &&
        (*(char *)(*(int *)(param_1 + 0x54) + 0x75) == '\0')) &&
       (*(char *)(*(int *)(param_1 + 0x58) + 0x75) == '\0')) {
      (**(code **)(**(int **)(*(int *)PTR_DAT_0045a28c + 0x48) + 0x6c))
                (*(int **)(*(int *)PTR_DAT_0045a28c + 0x48),0);
      *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a28c + 0x48) + 0x94) = 0;
      (**(code **)(**(int **)(*(int *)PTR_DAT_0045a28c + 0x44) + 0x6c))
                (*(int **)(*(int *)PTR_DAT_0045a28c + 0x44),0);
      (**(code **)(**(int **)(*(int *)PTR_DAT_0045a0cc + 0x44) + 0x6c))
                (*(int **)(*(int *)PTR_DAT_0045a0cc + 0x44),0);
      (**(code **)(**(int **)(param_1 + 0x4c) + 0x6c))(*(int **)(param_1 + 0x4c),0);
      (**(code **)(**(int **)(param_1 + 0x88) + 0x6c))(*(int **)(param_1 + 0x88),0);
      AoWE_TAoWEngine_CloseCampaign_23EDC2EF(*(undefined4 *)AoWE_AoWEngine_7E705837_exref);
      FUN_0041a128(*(undefined4 *)PTR_DAT_0045a434);
      FUN_0041a150(*(undefined4 *)PTR_DAT_0045a434,1);
    }
    break;
  case 2:
    iVar3 = FUN_0041bbb8(param_1);
    uVar4 = *(undefined4 *)(iVar3 + 0x28);
    uVar8 = FUN_0041bbb8(param_1);
    iVar3 = FUN_0040bde4(uVar8,uVar4);
    iVar3 = FUN_0040bbb8(*(undefined4 *)(iVar3 + 0x30),0);
    if (*(int *)(iVar3 + 8) == 0) {
      iVar3 = FUN_0041bbb8(param_1);
      uVar4 = *(undefined4 *)(iVar3 + 0x28);
      uVar8 = FUN_0041bbb8(param_1);
      iVar3 = FUN_0040bde4(uVar8,uVar4);
      iVar3 = FUN_0040bbb8(*(undefined4 *)(iVar3 + 0x30),0);
      iVar3 = (**(code **)(**(int **)(iVar3 + 0x14) + 0x14))();
      if (iVar3 < 1) {
        *(undefined4 *)(param_1 + 0x150) = 0x14;
        (**(code **)(**(int **)(param_1 + 0x88) + 0x6c))(*(int **)(param_1 + 0x88),0);
        (**(code **)(**(int **)(param_1 + 300) + 0x6c))(*(int **)(param_1 + 300),0);
        (**(code **)(**(int **)(param_1 + 0x100) + 0x6c))(*(int **)(param_1 + 0x100),0);
        (**(code **)(**(int **)(param_1 + 0xfc) + 0x6c))(*(int **)(param_1 + 0xfc),0);
        iVar3 = FUN_0041bbb8(param_1);
        *(int *)(iVar3 + 0x38) = *(int *)(iVar3 + 0x38) + 1;
        FUN_0041ca5c(param_1);
        break;
      }
    }
    (**(code **)(**(int **)(param_1 + 300) + 0x6c))(*(int **)(param_1 + 300),0);
    (**(code **)(**(int **)(param_1 + 0x100) + 0x6c))(*(int **)(param_1 + 0x100),0);
    (**(code **)(**(int **)(param_1 + 0xfc) + 0x6c))(*(int **)(param_1 + 0xfc),0);
    iVar3 = FUN_0041bbb8(param_1);
    FUN_0041c344(param_1,*(undefined4 *)(iVar3 + 0x28));
    break;
  case 3:
    FUN_0040cd08(*(undefined4 *)PTR_DAT_0045a0e0);
    (**(code **)(**(int **)(param_1 + 300) + 0x6c))(*(int **)(param_1 + 300),0);
    (**(code **)(**(int **)(param_1 + 0x100) + 0x6c))(*(int **)(param_1 + 0x100),0);
    (**(code **)(**(int **)(param_1 + 0xfc) + 0x6c))(*(int **)(param_1 + 0xfc),0);
    break;
  case 4:
    uVar4 = *(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x2c);
    pcVar10 = "Scenario\\Campaign\\";
    iVar3 = FUN_0041bbb8(param_1);
    uVar8 = *(undefined4 *)(iVar3 + 0x28);
    uVar11 = FUN_0041bbb8(param_1);
    iVar3 = FUN_0040bde4(uVar11,uVar8);
    uVar8 = *(undefined4 *)(iVar3 + 0x10);
    System__LStrCatN_51F89FF7(&local_8,3);
    iVar3 = **(int **)AoWE_AoWEngine_7E705837_exref;
    iVar3 = (**(code **)(iVar3 + 0xa8))
                      (*(int **)AoWE_AoWEngine_7E705837_exref,local_8,iVar3,uVar8,pcVar10,uVar4);
    if (iVar3 == 0) break;
    iVar5 = AoWE_TPlayerList_IndexOfPlayerType_23EDC2EF(*(undefined4 *)(iVar3 + 0x140),0);
    if ((iVar5 == -1) || (iVar6 = FUN_0041bbb8(param_1), *(int *)(*(int *)(iVar6 + 0x18) + 8) < 1))
    {
LAB_0041d647:
      iVar5 = FUN_0041bbb8(param_1);
      *(int *)(iVar5 + 0x38) = *(int *)(iVar5 + 0x38) + 1;
    }
    else {
      iVar5 = AoWE_TPlayerList_GetPlayers_23EDC2EF(*(undefined4 *)(iVar3 + 0x140),iVar5);
      uVar7 = (uint)*(byte *)(iVar5 + 0xa5);
      iVar5 = FUN_0041bbb8(param_1);
      iVar5 = Engine_TByteList_IndexOf_23EDC2EF(*(undefined4 *)(iVar5 + 0x18),uVar7);
      if (iVar5 != -1) goto LAB_0041d647;
      iVar5 = FUN_0041bbb8(param_1);
      iVar5 = AoWE_TLeader_GetSphereCount_23EDC2EF(*(undefined4 *)(*(int *)(iVar5 + 0x20) + 8));
      iVar6 = FUN_0041bbb8(param_1);
      if (*(int *)(*(int *)(iVar6 + 0x18) + 8) * 2 + 3 <= iVar5) goto LAB_0041d647;
      FUN_0041c89c(param_1);
    }
    System_TObject_Free_23EDC2EF(iVar3);
    break;
  case 7:
    iVar3 = FUN_0041bbb8(param_1);
    uVar4 = *(undefined4 *)(iVar3 + 0x28);
    uVar8 = FUN_0041bbb8(param_1);
    iVar3 = FUN_0040bde4(uVar8,uVar4);
    if (*(int *)(iVar3 + 0x10) == 0) {
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b0f8 + 100),0);
      iVar3 = FUN_0041bbb8(param_1);
      *(undefined4 *)(iVar3 + 0x38) = 0;
      FUN_0041ca5c(param_1);
    }
    else {
      iVar3 = FUN_0041bbb8(param_1);
      *(int *)(iVar3 + 0x38) = *(int *)(iVar3 + 0x38) + 1;
      uVar11 = 0;
      uVar8 = 0;
      iVar3 = FUN_0041bbb8(param_1);
      uVar4 = *(undefined4 *)(iVar3 + 0x28);
      piVar9 = (int *)FUN_0041bbb8(param_1);
      (**(code **)(*piVar9 + 0x5c))(piVar9,uVar4,*piVar9,uVar8,uVar11);
      System__LStrCat3_51F89FF7
                (&local_8,*(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x2c),
                 "Scenario\\Campaign\\");
      SysUtils_SetCurrentDir_3C9BDBFB(local_8);
      iVar3 = FUN_0041bbb8(param_1);
      uVar4 = *(undefined4 *)(iVar3 + 0x24);
      uVar8 = FUN_0041bbb8(param_1);
      iVar3 = FUN_0040bde4(uVar8,uVar4);
      FUN_00454b70(*(undefined4 *)PTR_DAT_0045a420,*(undefined4 *)(iVar3 + 0x10));
      (**(code **)(**(int **)(*(int *)PTR_DAT_0045a28c + 0x48) + 0x6c))
                (*(int **)(*(int *)PTR_DAT_0045a28c + 0x48),1);
      (**(code **)(**(int **)(param_1 + 0x4c) + 0x6c))(*(int **)(param_1 + 0x4c),0);
      piVar9 = (int *)FUN_0041bbb8(param_1);
      (**(code **)(*piVar9 + 0x60))();
      FUN_00454d2c(*(undefined4 *)PTR_DAT_0045a420);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b0f8 + 100),0);
    }
    break;
  case 10:
    iVar3 = FUN_0041bbb8(param_1);
    FUN_0041c344(param_1,*(undefined4 *)(iVar3 + 0x24));
  }
switchD_0041ca96_caseD_5:
  puVar2 = puStack_18;
  *in_FS_OFFSET = uStack_20;
  puStack_18 = &LAB_0041d785;
  puStack_1c = (undefined1 *)0x41d77d;
  System__LStrClr_40929B27(&local_8,uStack_20,puVar2);
  return;
}

