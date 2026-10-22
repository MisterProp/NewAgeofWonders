
void FUN_0041f430(int param_1)

{
  int *piVar1;
  undefined1 *puVar2;
  int iVar3;
  int iVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 *in_FS_OFFSET;
  undefined1 uVar7;
  bool bVar8;
  float10 in_ST0;
  float10 fVar9;
  float10 in_ST1;
  float10 in_ST2;
  float10 in_ST3;
  float10 in_ST4;
  float10 in_ST5;
  float10 in_ST6;
  float10 in_ST7;
  undefined4 *puVar10;
  undefined4 uStack_188;
  undefined1 *puStack_184;
  undefined1 *puStack_180;
  undefined4 local_170;
  undefined4 local_16c [2];
  uint local_164;
  undefined4 local_160;
  int local_14;
  undefined4 local_10;
  undefined4 local_c;
  char local_5;
  
  local_170 = 0;
  local_c = 0;
  local_10 = 0;
  puStack_180 = (undefined1 *)0x41f45d;
  System__InitializeRecord_51F89FF7(local_16c,SysUtils_TSearchRec_08A7EF01_exref);
  puStack_184 = &LAB_0041f908;
  uStack_188 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_188;
  puStack_180 = &stack0xfffffffc;
  iVar3 = (**(code **)(**(int **)(*(int *)(param_1 + 0xe8) + 0x114) + 0x14))();
  if (-1 < iVar3 + -1) {
    local_14 = 0;
    do {
      piVar1 = *(int **)(*(int *)(param_1 + 0xe8) + 0x114);
      iVar4 = (**(code **)(*piVar1 + 0x18))(piVar1,local_14);
      if (iVar4 != 0) {
        piVar1 = *(int **)(*(int *)(param_1 + 0xe8) + 0x114);
        (**(code **)(*piVar1 + 0x18))(piVar1,local_14);
        System_TObject_Free_23EDC2EF();
      }
      local_14 = local_14 + 1;
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
  }
  (**(code **)(*DAT_0045b110 + 0x40))();
  (**(code **)(*DAT_0045b10c + 0x40))();
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 200),0);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xcc),0);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xd0),0);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x98),0);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x9c),0);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xa0),0);
  uVar7 = 1;
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xa4));
  SysUtils_GetCurrentDir_6835DD73(&local_10);
  puVar10 = &local_170;
  uVar5 = System__LStrLen_C43AB84F(local_10);
  System__LStrCopy_51F89FF7(local_10,uVar5,1,puVar10);
  System__LStrCmp_51F89FF7(local_170,&DAT_0041f920);
  if ((bool)uVar7) {
    puVar10 = &local_10;
    iVar3 = System__LStrLen_C43AB84F(local_10);
    System__LStrCopy_51F89FF7(local_10,1,iVar3 + -1,puVar10);
  }
  System__LStrCat3_51F89FF7(&local_170,local_10,&DAT_0041f92c);
  iVar3 = SysUtils_FindFirst_1E70405E(local_170,0x10,local_16c);
  do {
    bVar8 = iVar3 == 0;
    if (!bVar8) {
      SysUtils_FindClose_1671C198(local_16c);
      AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x58),0xffffffff);
      AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0xd8),0xffffffff);
      AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0xe8),0xffffffff);
      AoWListBox_TAOWListBox_SetListOff_23EDC2EF(*(undefined4 *)(param_1 + 0x58),0);
      AoWListBox_TAOWListBox_SetListOff_23EDC2EF(*(undefined4 *)(param_1 + 0xd8),0);
      AoWListBox_TAOWListBox_SetListOff_23EDC2EF(*(undefined4 *)(param_1 + 0xe8),0);
      (**(code **)(**(int **)(*(int *)(param_1 + 0x58) + 0x114) + 0x40))();
      (**(code **)(**(int **)(*(int *)(param_1 + 0xd8) + 0x114) + 0x40))();
      (**(code **)(**(int **)(*(int *)(param_1 + 0xe8) + 0x114) + 0x40))();
      (**(code **)(*DAT_0045b110 + 0x7c))();
      (**(code **)(*DAT_0045b10c + 0x7c))();
      piVar1 = *(int **)(*(int *)(param_1 + 0x58) + 0x114);
      (**(code **)(*piVar1 + 0x3c))(piVar1,DAT_0045b110);
      piVar1 = *(int **)(*(int *)(param_1 + 0x58) + 0x114);
      (**(code **)(*piVar1 + 0x3c))(piVar1,DAT_0045b10c);
      piVar1 = *(int **)(*(int *)(param_1 + 0xd8) + 0x114);
      (**(code **)(*piVar1 + 0x3c))(piVar1,*(undefined4 *)(*(int *)(param_1 + 0x58) + 0x114));
      System__LStrCat3_51F89FF7(&local_170,local_10,&DAT_0041f920);
      AoWEdit_TAOWEdit_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x104),local_170);
      iVar3 = (**(code **)(*DAT_0045b110 + 0x14))();
      if (-1 < iVar3 + -1) {
        do {
          piVar1 = *(int **)(*(int *)(param_1 + 0xe8) + 0x114);
          (**(code **)(*piVar1 + 0x34))(piVar1,0);
          iVar3 = iVar3 + -1;
        } while (iVar3 != 0);
      }
      iVar3 = (**(code **)(*DAT_0045b10c + 0x14))();
      if (-1 < iVar3 + -1) {
        local_14 = 0;
        do {
          (**(code **)(*DAT_0045b10c + 0x18))(DAT_0045b10c,local_14);
          SysUtils_DateTimeToStr_0B3E54BB(&local_170);
          uVar5 = local_170;
          uVar6 = (**(code **)(*DAT_0045b10c + 0x18))(DAT_0045b10c,local_14);
          piVar1 = *(int **)(*(int *)(param_1 + 0xe8) + 0x114);
          (**(code **)(*piVar1 + 0x38))(piVar1,uVar5,uVar6);
          local_14 = local_14 + 1;
          iVar3 = iVar3 + -1;
        } while (iVar3 != 0);
      }
      puVar2 = puStack_180;
      *in_FS_OFFSET = uStack_188;
      puStack_180 = &LAB_0041f90f;
      puStack_184 = (undefined1 *)0x41f8e9;
      System__LStrClr_40929B27(&local_170,uStack_188,puVar2);
      puStack_184 = (undefined1 *)0x41f8fa;
      System__FinalizeRecord_51F89FF7(local_16c,SysUtils_TSearchRec_08A7EF01_exref);
      puStack_184 = (undefined1 *)0x41f907;
      System__LStrArrayClr_51F89FF7(&local_10,2);
      return;
    }
    System__LStrCmp_51F89FF7(local_160,&DAT_0041f938);
    fVar9 = in_ST0;
    if ((!bVar8) && (System__LStrCmp_51F89FF7(local_160,&DAT_0041f944), fVar9 = in_ST0, !bVar8)) {
      if ((local_164 & 0x10) == 0x10) {
        (**(code **)(*DAT_0045b110 + 0x34))(DAT_0045b110,local_160);
        fVar9 = in_ST0;
      }
      else {
        local_5 = '\0';
        SysUtils_ExtractFileExt_435799AA(local_160,&local_170);
        SysUtils_LowerCase_435799AA(local_170,&local_c);
        if ((local_5 != '\0') ||
           (((*(int *)(param_1 + 0x148) == 1 &&
             (bVar8 = *(int *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x70) == 0, !bVar8)) &&
            (System__LStrCmp_51F89FF7(local_c,&DAT_0041f950), bVar8)))) {
          local_5 = '\x01';
        }
        else {
          local_5 = '\0';
        }
        if ((local_5 != '\0') ||
           (((*(int *)(param_1 + 0x148) == 1 &&
             (bVar8 = *(int *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x70) == 0, bVar8)) &&
            (System__LStrCmp_51F89FF7(local_c,&DAT_0041f960), bVar8)))) {
          local_5 = '\x01';
        }
        else {
          local_5 = '\0';
        }
        if ((local_5 != '\0') ||
           ((bVar8 = *(int *)(param_1 + 0x148) == 0, bVar8 &&
            ((System__LStrCmp_51F89FF7(local_c,&DAT_0041f960), bVar8 ||
             (System__LStrCmp_51F89FF7(local_c,&DAT_0041f950), bVar8)))))) {
          local_5 = '\x01';
        }
        else {
          local_5 = '\0';
        }
        if ((local_5 != '\0') ||
           ((bVar8 = *(int *)(param_1 + 0x148) == 2, bVar8 &&
            (System__LStrCmp_51F89FF7(local_c,&LAB_0041f970), bVar8)))) {
LAB_0041f6f7:
          local_5 = '\x01';
        }
        else {
          bVar8 = false;
          System__LStrCmp_51F89FF7(local_c,&DAT_0041f960);
          if (bVar8) goto LAB_0041f6f7;
          local_5 = '\0';
        }
        fVar9 = in_ST0;
        if (local_5 != '\0') {
          iVar3 = System_TObject_Create_23EDC2EF(&DAT_0041eb14,1);
          fVar9 = in_ST1;
          in_ST1 = in_ST2;
          in_ST2 = in_ST3;
          in_ST3 = in_ST4;
          in_ST4 = in_ST5;
          in_ST5 = in_ST6;
          in_ST6 = in_ST7;
          SysUtils_FileDateToDateTime_D7569B07(local_16c[0]);
          *(double *)(iVar3 + 4) = (double)in_ST0;
          in_ST7 = in_ST6;
          (**(code **)(*DAT_0045b10c + 0x38))(DAT_0045b10c,local_160,iVar3);
        }
      }
    }
    iVar3 = SysUtils_FindNext_51BC3B50(local_16c);
    in_ST0 = fVar9;
  } while( true );
}

