
void FUN_0042058c(int param_1)

{
  undefined1 *puVar1;
  int iVar2;
  int iVar3;
  undefined4 *in_FS_OFFSET;
  undefined1 uVar4;
  undefined4 *puVar5;
  undefined4 uStack_20;
  undefined1 *puStack_1c;
  undefined1 *puStack_18;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_18 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  puStack_1c = &LAB_004208e5;
  uStack_20 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_20;
  iVar2 = *(int *)(param_1 + 0x148);
  if (iVar2 == 0) {
    SysUtils_SetCurrentDir_3C9BDBFB(*(undefined4 *)(param_1 + 0x134));
    SysUtils_ExtractFileName_435799AA(*(undefined4 *)(param_1 + 0x138),&local_8);
    AoWEdit_TAOWEdit_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xdc),local_8);
    AoWEdit_TAOWEdit_SetGText_23EDC2EF
              (*(undefined4 *)(param_1 + 0x5c),*(undefined4 *)(*(int *)(param_1 + 0xdc) + 0x90));
    System_LoadResString_02614E1B(AoWE_SelectAFileToLoadRStr_04ECFCC5_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_8);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b104 + 0x54),local_8);
    *(undefined1 *)(*(int *)(DAT_0045b104 + 0xdc) + 0x130) = 0;
    System_LoadResString_02614E1B(AoWE_OpenRStr_04ECFCC5_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_8);
    AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b104 + 0x50),local_8);
    (**(code **)(**(int **)(param_1 + 0xe4) + 0x6c))(*(int **)(param_1 + 0xe4),1);
    (**(code **)(**(int **)(param_1 + 0xd4) + 0x6c))(*(int **)(param_1 + 0xd4),0);
  }
  else if (iVar2 == 1) {
    puStack_18 = &stack0xfffffffc;
    SysUtils_SetCurrentDir_3C9BDBFB(*(undefined4 *)(param_1 + 0x134));
    SysUtils_ExtractFileName_435799AA(*(undefined4 *)(param_1 + 0x138),&local_8);
    AoWEdit_TAOWEdit_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xdc),local_8);
    iVar2 = System__LStrPos_51F89FF7(&DAT_004208fc,*(undefined4 *)(*(int *)(param_1 + 0xdc) + 0x90))
    ;
    if (iVar2 != 0) {
      puVar5 = &local_8;
      iVar3 = System__LStrPos_51F89FF7
                        (&DAT_004208fc,*(undefined4 *)(*(int *)(param_1 + 0xdc) + 0x90));
      iVar2 = *(int *)(param_1 + 0xdc);
      System__LStrCopy_51F89FF7(*(undefined4 *)(iVar2 + 0x90),1,iVar3 + -1,puVar5);
      AoWEdit_TAOWEdit_SetGText_23EDC2EF(iVar2,local_8);
    }
    iVar2 = *(int *)(param_1 + 0xdc);
    if (*(int *)(iVar2 + 0x90) != 0) {
      if (*(int *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x70) == 0) {
        System__LStrCat3_51F89FF7
                  (&local_8,*(undefined4 *)(*(int *)(param_1 + 0xdc) + 0x90),&DAT_00420918);
        AoWEdit_TAOWEdit_SetGText_23EDC2EF(iVar2,local_8);
      }
      else {
        System__LStrCat3_51F89FF7
                  (&local_8,*(undefined4 *)(*(int *)(param_1 + 0xdc) + 0x90),&DAT_00420908);
        AoWEdit_TAOWEdit_SetGText_23EDC2EF(iVar2,local_8);
      }
    }
    AoWEdit_TAOWEdit_SetGText_23EDC2EF
              (*(undefined4 *)(param_1 + 0x5c),*(undefined4 *)(*(int *)(param_1 + 0xdc) + 0x90));
    System_LoadResString_02614E1B(AoWE_SaveGameAsRStr_04ECFCC5_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_8);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b104 + 0x54),local_8);
    *(undefined1 *)(*(int *)(DAT_0045b104 + 0xdc) + 0x130) = 1;
    System_LoadResString_02614E1B(AoWE_SaveRStr_04ECFCC5_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_8);
    AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b104 + 0x50),local_8);
    (**(code **)(**(int **)(param_1 + 0xe4) + 0x6c))(*(int **)(param_1 + 0xe4),1);
    (**(code **)(**(int **)(param_1 + 0xd4) + 0x6c))(*(int **)(param_1 + 0xd4),0);
  }
  else {
    uVar4 = iVar2 == 2;
    puStack_18 = &stack0xfffffffc;
    if ((bool)uVar4) {
      puStack_18 = &stack0xfffffffc;
      SysUtils_ExtractFileExt_435799AA(*(undefined4 *)(param_1 + 0x138),&local_8);
      System__LStrCmp_51F89FF7(local_8,&DAT_00420928);
      if ((bool)uVar4) {
        SysUtils_ExtractFileName_435799AA(*(undefined4 *)(param_1 + 0x138),&local_8);
        AoWEdit_TAOWEdit_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x5c),local_8);
      }
      else {
        AoWEdit_TAOWEdit_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x5c),0);
      }
      System__LStrCat3_51F89FF7
                (&local_8,*(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x2c),
                 "Scenario\\Custom\\");
      SysUtils_SetCurrentDir_3C9BDBFB(local_8);
      System_LoadResString_02614E1B(AoWE_SelectAMapRStr_04ECFCC5_exref,&local_c);
      AoWE_TranslateRStr_435799AA(local_c,&local_8);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b104 + 0x54),local_8);
      System_LoadResString_02614E1B(AoWE_SelectRStr_04ECFCC5_exref,&local_c);
      AoWE_TranslateRStr_435799AA(local_c,&local_8);
      AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b104 + 0x50),local_8);
      (**(code **)(**(int **)(param_1 + 0xe4) + 0x6c))(*(int **)(param_1 + 0xe4),0);
      (**(code **)(**(int **)(param_1 + 0xd4) + 0x6c))(*(int **)(param_1 + 0xd4),1);
    }
  }
  FUN_0041f1f4(DAT_0045b104);
  puVar1 = puStack_18;
  *in_FS_OFFSET = uStack_20;
  puStack_18 = &LAB_004208ec;
  puStack_1c = (undefined1 *)0x4208e4;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar1);
  return;
}

