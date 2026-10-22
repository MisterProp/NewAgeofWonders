
void FUN_00414208(undefined4 param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  undefined4 *in_FS_OFFSET;
  undefined4 uStackY_44;
  undefined4 uVar2;
  undefined4 *puVar3;
  undefined4 uStack_2c;
  undefined1 *puStack_28;
  undefined1 *puStack_24;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_24 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  local_10 = 0;
  local_14 = 0;
  local_18 = 0;
  puStack_28 = &LAB_004143cf;
  uStack_2c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_2c;
  SysUtils_ExtractFileName_435799AA(param_3,&local_14,param_3,&local_8);
  local_10 = local_14;
  local_c = CONCAT31(local_c._1_3_,0xb);
  puVar3 = &local_10;
  System_LoadResString_02614E1B(AoWE_LoadingXRStr_04ECFCC5_exref,&local_8);
  AoWE_TranslateRStr_435799AA(local_8,&local_18);
  AoWE_Format_D8F48273(local_18,puVar3,0);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(*(int *)PTR_DAT_0045a24c + 0x44),local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(*(int *)PTR_DAT_0045a24c + 0x50),0);
  AOWGauge_TAoWGauge_SetPos_23EDC2EF(*(undefined4 *)(*(int *)PTR_DAT_0045a24c + 0x58),0);
  *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a24c + 0x58) + 0x118) = 100;
  (**(code **)(**(int **)(*(int *)PTR_DAT_0045a24c + 0x54) + 0x6c))
            (*(int **)(*(int *)PTR_DAT_0045a24c + 0x54),1);
  AOWWinManager_TAOWWinManager_SetOverPri_23EDC2EF
            (*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0x44),5000);
  if (*(char *)(*(int *)(param_2 + 0x28) + 0x2a) == '\0') {
    Sound_TSongPlayer_Stop_23EDC2EF(*(undefined4 *)Sound_SongPlayer_F3DCC77F_exref,0);
    (**(code **)(**(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0xc) + 0x40))();
    Sound_FadeMusicVolume_2796EB69(0,1);
  }
  iVar1 = Classes_TStream_GetSize_23EDC2EF(*(undefined4 *)(param_2 + 0x2c));
  if (iVar1 == 0) {
    uVar2 = 0x414383;
    (**(code **)(**(int **)AoWE_AoWEngine_7E705837_exref + 0xa4))
              (*(int **)AoWE_AoWEngine_7E705837_exref,param_3,0,&LAB_0045540c,
               *(undefined4 *)PTR_DAT_0045a420);
  }
  else {
    HSEngine_THSEngine_LoadHSMfromStream_23EDC2EF
              (*(undefined4 *)AoWE_AoWEngine_7E705837_exref,*(undefined4 *)(param_2 + 0x2c),0,
               &LAB_0045540c,*(undefined4 *)PTR_DAT_0045a420);
    uVar2 = *(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x2c);
    SysUtils_ExtractFileName_435799AA(param_3,&local_8);
    uStackY_44 = local_8;
    System__LStrCatN_51F89FF7(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x7c,3);
  }
  AOWWinManager_TAOWWinManager_SetOverPri_23EDC2EF
            (*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0x44),0);
  (**(code **)(**(int **)(*(int *)PTR_DAT_0045a24c + 0x54) + 0x6c))
            (*(int **)(*(int *)PTR_DAT_0045a24c + 0x54),0);
  AoWSetup_TSetupControl_MapLoaded_23EDC2EF(param_2);
  *in_FS_OFFSET = uStackY_44;
  System__LStrArrayClr_51F89FF7(&local_18,2,uVar2,&LAB_004143d6);
  System__LStrClr_40929B27(&local_8);
  return;
}

