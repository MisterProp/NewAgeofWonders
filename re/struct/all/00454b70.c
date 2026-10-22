
void FUN_00454b70(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  undefined *puVar2;
  undefined4 *in_FS_OFFSET;
  undefined1 *puVar3;
  undefined *puVar4;
  undefined4 uVar5;
  undefined4 uStack_2c;
  undefined1 *puStack_28;
  undefined1 *puStack_24;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  
  local_c = 0;
  local_10 = 0;
  local_14 = 0;
  puStack_24 = (undefined1 *)0x454b89;
  local_8 = param_2;
  System__LStrAddRef_51F89FF7(param_2);
  puVar2 = PTR_DAT_0045a24c;
  puStack_28 = &LAB_00454d03;
  uStack_2c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_2c;
  puStack_24 = &stack0xfffffffc;
  if (*(int *)AoWE_AoWHSMap_6B957A58_exref != 0) {
    puStack_24 = &stack0xfffffffc;
    (**(code **)(**(int **)AoWE_AoWEngine_7E705837_exref + 0xb0))();
  }
  System_LoadResString_02614E1B(PTR_PTR_0045a0ec,&local_14);
  AoWE_TranslateRStr_435799AA(local_14,&local_10);
  SysUtils_ExtractFileName_435799AA(local_8,&local_14);
  puVar4 = &DAT_00454d28;
  uVar5 = local_14;
  System__LStrCatN_51F89FF7(&local_c,4);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(*(int *)puVar2 + 0x44),local_c);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(*(int *)puVar2 + 0x50),0);
  AOWGauge_TAoWGauge_SetPos_23EDC2EF(*(undefined4 *)(*(int *)puVar2 + 0x58),0);
  *(undefined4 *)(*(int *)(*(int *)puVar2 + 0x58) + 0x118) = 100;
  iVar1 = **(int **)(*(int *)puVar2 + 0x54);
  (**(code **)(iVar1 + 0x6c))(*(int **)(*(int *)puVar2 + 0x54),1,iVar1,puVar4,uVar5);
  if (*(char *)(*(int *)(*(int *)PTR_DAT_0045a434 + 0x48) + 0x75) == '\0') {
    AOWWinManager_TAOWWinManager_SetOverPri_23EDC2EF(*(undefined4 *)(DAT_0045b2c0 + 0x44),5000);
  }
  Sound_TSongPlayer_Stop_23EDC2EF(*(undefined4 *)Sound_SongPlayer_F3DCC77F_exref,0);
  (**(code **)(**(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0xc) + 0x40))();
  Sound_FadeMusicVolume_2796EB69(0,1);
  puVar3 = &LAB_0045540c;
  (**(code **)(**(int **)AoWE_AoWEngine_7E705837_exref + 0xa4))
            (*(int **)AoWE_AoWEngine_7E705837_exref,local_8,0,&LAB_0045540c,param_1);
  if (((*(char *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x6e) != '\0') &&
      (*(int *)AoWE_AoWHSMap_6B957A58_exref != 0)) &&
     (*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x191) == '\0')) {
    FUN_0045530c(param_1);
  }
  AOWWinManager_TAOWWinManager_SetOverPri_23EDC2EF(*(undefined4 *)(DAT_0045b2c0 + 0x44),0);
  (**(code **)(**(int **)(*(int *)puVar2 + 0x54) + 0x6c))(*(int **)(*(int *)puVar2 + 0x54),0);
  *in_FS_OFFSET = puVar3;
  System__LStrArrayClr_51F89FF7(&local_14,4,puVar4,&LAB_00454d0a);
  return;
}

