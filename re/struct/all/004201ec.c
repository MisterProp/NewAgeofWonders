
void FUN_004201ec(int param_1,int param_2)

{
  int *piVar1;
  undefined1 *puVar2;
  int iVar3;
  int iVar4;
  undefined4 *in_FS_OFFSET;
  undefined1 uVar5;
  undefined4 uStack_20;
  undefined1 *puStack_1c;
  undefined1 *puStack_18;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_18 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  puStack_1c = &LAB_00420523;
  uStack_20 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_20;
  AoWListBox_TAOWListBox_SetIndex_23EDC2EF
            (*(undefined4 *)(param_1 + 0x58),*(undefined4 *)(param_2 + 0x120));
  AoWListBox_TAOWListBox_SetIndex_23EDC2EF
            (*(undefined4 *)(param_1 + 0xd8),*(undefined4 *)(param_2 + 0x120));
  AoWListBox_TAOWListBox_SetIndex_23EDC2EF
            (*(undefined4 *)(param_1 + 0xe8),*(undefined4 *)(param_2 + 0x120));
  iVar3 = (**(code **)(*DAT_0045b110 + 0x14))();
  iVar4 = *(int *)(*(int *)(param_1 + 0x58) + 0x120);
  uVar5 = iVar3 + -1 == iVar4;
  if (iVar3 + -1 < iVar4) {
    piVar1 = *(int **)(*(int *)(param_1 + 0x58) + 0x114);
    (**(code **)(*piVar1 + 0xc))(piVar1,*(undefined4 *)(*(int *)(param_1 + 0x58) + 0x120),&local_8);
    AoWEdit_TAOWEdit_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xdc),local_8);
    SysUtils_ExtractFileExt_435799AA(*(undefined4 *)(*(int *)(param_1 + 0xdc) + 0x90),&local_c);
    SysUtils_LowerCase_435799AA(local_c,&local_8);
    System__LStrCmp_51F89FF7(local_8,&DAT_00420538);
    if ((bool)uVar5) {
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x98),0);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x9c),0);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xa0),0);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xa4),0);
    }
    else {
      iVar4 = (**(code **)(**(int **)AoWE_AoWEngine_7E705837_exref + 0xa8))
                        (*(int **)AoWE_AoWEngine_7E705837_exref,
                         *(undefined4 *)(*(int *)(param_1 + 0xdc) + 0x90));
      if (iVar4 == 0) {
        AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x98),0);
        AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x9c),0);
        AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xa0),0);
        AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xa4),0);
      }
      else {
        iVar3 = (**(code **)(**(int **)(iVar4 + 0x140) + 0x54))();
        SysUtils_IntToStr_0F6FDFF6(iVar3 + -1,&local_8);
        AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x98),local_8);
        iVar3 = *(int *)(*(int *)(iVar4 + 0x10) + 0xc);
        if (iVar3 == 0x30) {
          System_LoadResString_02614E1B(AoWE_SmallMapAbrRStr_04ECFCC5_exref,&local_c);
          AoWE_TranslateRStr_435799AA(local_c,&local_8);
          AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x9c),local_8);
        }
        else if (iVar3 == 0x40) {
          System_LoadResString_02614E1B(AoWE_MediumMapAbrRStr_04ECFCC5_exref,&local_c);
          AoWE_TranslateRStr_435799AA(local_c,&local_8);
          AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x9c),local_8);
        }
        else if (iVar3 == 0x60) {
          System_LoadResString_02614E1B(AoWE_LargeMapAbrRStr_04ECFCC5_exref,&local_c);
          AoWE_TranslateRStr_435799AA(local_c,&local_8);
          AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x9c),local_8);
        }
        else if (iVar3 - 0x7fU < 2) {
          System_LoadResString_02614E1B(AoWE_ExtraLargeMapAbrRStr_04ECFCC5_exref,&local_c);
          AoWE_TranslateRStr_435799AA(local_c,&local_8);
          AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x9c),local_8);
        }
        else {
          AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x9c),&LAB_00420548);
        }
        SysUtils_IntToStr_0F6FDFF6(*(undefined4 *)(*(int *)(iVar4 + 0x100) + 0x10),&local_8);
        AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xa0),local_8);
        SysUtils_IntToStr_0F6FDFF6(*(undefined4 *)(iVar4 + 0x174),&local_8);
        AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xa4),local_8);
        System_TObject_Free_23EDC2EF(iVar4);
      }
    }
  }
  else {
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x98),0);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x9c),0);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xa0),0);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xa4),0);
  }
  AoWEdit_TAOWEdit_SetGText_23EDC2EF
            (*(undefined4 *)(param_1 + 0x5c),*(undefined4 *)(*(int *)(param_1 + 0xdc) + 0x90));
  AoWLabel_TAOWLabel_SetGText_23EDC2EF
            (*(undefined4 *)(param_1 + 200),*(undefined4 *)(*(int *)(param_1 + 0x98) + 0x90));
  AoWLabel_TAOWLabel_SetGText_23EDC2EF
            (*(undefined4 *)(param_1 + 0xcc),*(undefined4 *)(*(int *)(param_1 + 0x9c) + 0x90));
  AoWLabel_TAOWLabel_SetGText_23EDC2EF
            (*(undefined4 *)(param_1 + 0xd0),*(undefined4 *)(*(int *)(param_1 + 0xa0) + 0x90));
  puVar2 = puStack_18;
  *in_FS_OFFSET = uStack_20;
  puStack_18 = &LAB_0042052a;
  puStack_1c = (undefined1 *)0x420522;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar2);
  return;
}

