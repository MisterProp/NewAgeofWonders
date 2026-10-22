
void FUN_00454940(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 *in_FS_OFFSET;
  undefined1 *puVar2;
  undefined4 uVar3;
  undefined4 uStack_24;
  undefined1 *puStack_20;
  undefined1 *puStack_1c;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  
  local_c = 0;
  local_10 = 0;
  local_14 = 0;
  puStack_1c = (undefined1 *)0x454957;
  local_8 = param_2;
  System__LStrAddRef_51F89FF7(param_2);
  puStack_20 = &LAB_00454afe;
  uStack_24 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_24;
  puStack_1c = &stack0xfffffffc;
  if (*(int *)AoWE_AoWHSMap_6B957A58_exref != 0) {
    puStack_1c = &stack0xfffffffc;
    (**(code **)(**(int **)AoWE_AoWEngine_7E705837_exref + 0xb0))();
  }
  AoWE_TAoWEngine_LoadCampaign_23EDC2EF(*(undefined4 *)AoWE_AoWEngine_7E705837_exref,local_8);
  if (*(int *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x70) != 0) {
    if (*(char *)(*(int *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x70) + 0x1c) == '\0') {
      FUN_0041a304(*(undefined4 *)PTR_DAT_0045a434,AoWE_AoWEngine_7E705837_exref);
      FUN_0041de44(*(undefined4 *)PTR_DAT_0045a174);
    }
    else {
      System_LoadResString_02614E1B(PTR_PTR_0045a0ec,&local_14);
      AoWE_TranslateRStr_435799AA(local_14,&local_10);
      SysUtils_ExtractFileName_435799AA(local_8,&local_14);
      puVar2 = &LAB_00454b20;
      uVar3 = local_14;
      System__LStrCatN_51F89FF7(&local_c,4);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(*(int *)PTR_DAT_0045a24c + 0x44),local_c)
      ;
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(*(int *)PTR_DAT_0045a24c + 0x50),0);
      AOWGauge_TAoWGauge_SetPos_23EDC2EF(*(undefined4 *)(*(int *)PTR_DAT_0045a24c + 0x58),0);
      *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a24c + 0x58) + 0x118) = 100;
      iVar1 = **(int **)(*(int *)PTR_DAT_0045a24c + 0x54);
      (**(code **)(iVar1 + 0x6c))(*(int **)(*(int *)PTR_DAT_0045a24c + 0x54),1,iVar1,puVar2,uVar3);
      if (*(char *)(*(int *)(*(int *)PTR_DAT_0045a434 + 0x48) + 0x75) == '\0') {
        AOWWinManager_TAOWWinManager_SetOverPri_23EDC2EF(*(undefined4 *)(DAT_0045b2c0 + 0x44),5000);
      }
      Sound_TSongPlayer_Stop_23EDC2EF(*(undefined4 *)Sound_SongPlayer_F3DCC77F_exref,0);
      (**(code **)(**(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0xc) + 0x40))();
      Sound_FadeMusicVolume_2796EB69(0,1);
      puVar2 = &LAB_0045540c;
      AoWE_TAoWEngine_LoadCampaignMap_23EDC2EF(*(undefined4 *)AoWE_AoWEngine_7E705837_exref);
      AOWWinManager_TAOWWinManager_SetOverPri_23EDC2EF(*(undefined4 *)(DAT_0045b2c0 + 0x44),0);
      iVar1 = **(int **)(*(int *)PTR_DAT_0045a24c + 0x54);
      (**(code **)(iVar1 + 0x6c))(*(int **)(*(int *)PTR_DAT_0045a24c + 0x54),0,iVar1,puVar2,param_1)
      ;
    }
  }
  puVar2 = puStack_1c;
  *in_FS_OFFSET = uStack_24;
  puStack_1c = &LAB_00454b05;
  puStack_20 = (undefined1 *)0x454afd;
  System__LStrArrayClr_51F89FF7(&local_14,4,puVar2);
  return;
}

