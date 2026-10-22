
void FUN_0041fa60(undefined4 param_1,undefined4 param_2)

{
  undefined1 *puVar1;
  undefined4 *in_FS_OFFSET;
  undefined1 uVar2;
  undefined4 uStack_34;
  undefined1 *puStack_30;
  undefined1 *puStack_2c;
  undefined4 uStack_28;
  undefined4 uStack_24;
  undefined1 *puStack_20;
  undefined1 *puStack_1c;
  undefined1 *puStack_18;
  undefined4 local_10;
  undefined4 local_c;
  int local_8;
  
  puStack_18 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  local_10 = 0;
  uVar2 = 1;
  puStack_1c = &LAB_0041fbe9;
  puStack_20 = (undefined1 *)*in_FS_OFFSET;
  *in_FS_OFFSET = &puStack_20;
  uStack_24 = 0x41fa86;
  AoWE_TAoWEngine_CloseCampaign_23EDC2EF(*(undefined4 *)AoWE_AoWEngine_7E705837_exref);
  uStack_24 = 0;
  uStack_28 = 0;
  puStack_2c = (undefined1 *)0x41fa99;
  (**(code **)(**(int **)AoWE_AoWEngine_7E705837_exref + 0xb0))();
  puStack_2c = (undefined1 *)0x41faa3;
  SysUtils_ExtractFileExt_435799AA(param_2,&local_10);
  puStack_2c = (undefined1 *)0x41faae;
  SysUtils_LowerCase_435799AA(local_10,&local_c);
  puStack_2c = (undefined1 *)0x41fabb;
  System__LStrCmp_51F89FF7(local_c,&DAT_0041fc00);
  if ((bool)uVar2) {
    if (*(char *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x6e) == '\0') {
      puStack_2c = (undefined1 *)0x41fadc;
      FUN_00454940(*(undefined4 *)PTR_DAT_0045a420,param_2);
      if (*(int *)AoWE_AoWHSMap_6B957A58_exref != 0) {
        puStack_2c = (undefined1 *)0x41faf6;
        FUN_00454d2c(*(undefined4 *)PTR_DAT_0045a420);
      }
    }
  }
  else {
    puStack_2c = (undefined1 *)0x41fb07;
    AoWE_TAoWEngine_CloseCampaign_23EDC2EF(*(undefined4 *)AoWE_AoWEngine_7E705837_exref);
    puStack_2c = (undefined1 *)0x41fb18;
    local_8 = (**(code **)(**(int **)AoWE_AoWEngine_7E705837_exref + 0xa8))
                        (*(int **)AoWE_AoWEngine_7E705837_exref,param_2);
    if (local_8 != 0) {
      puStack_30 = &LAB_0041fbc7;
      uStack_34 = *in_FS_OFFSET;
      *in_FS_OFFSET = &uStack_34;
      if (*(char *)(local_8 + 0x13a) == '\x01') {
        puStack_2c = &stack0xfffffffc;
        FUN_0041a304(*(undefined4 *)PTR_DAT_0045a434);
        FUN_00455a34(*(undefined4 *)PTR_DAT_0045a420);
        (**(code **)(**(int **)(*(int *)PTR_DAT_0045a0cc + 0x44) + 0x6c))
                  (*(int **)(*(int *)PTR_DAT_0045a0cc + 0x44),0);
        SysUtils_ExtractRelativePath_8B0046BD
                  (*(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x2c),param_2,&local_c);
        FUN_0042abf0(*(undefined4 *)PTR_DAT_0045a598,local_c);
      }
      else {
        puStack_2c = &stack0xfffffffc;
        FUN_00454b70(*(undefined4 *)PTR_DAT_0045a420,param_2);
        if (*(int *)AoWE_AoWHSMap_6B957A58_exref != 0) {
          FUN_00454d2c(*(undefined4 *)PTR_DAT_0045a420);
        }
      }
      puVar1 = puStack_2c;
      *in_FS_OFFSET = uStack_34;
      puStack_2c = (undefined1 *)0x41fbce;
      puStack_30 = (undefined1 *)0x41fbc6;
      System_TObject_Free_23EDC2EF(local_8,uStack_34,puVar1);
      return;
    }
  }
  puVar1 = puStack_20;
  *in_FS_OFFSET = uStack_28;
  puStack_20 = &LAB_0041fbf0;
  uStack_24 = 0x41fbe8;
  System__LStrArrayClr_51F89FF7(&local_10,2,puVar1);
  return;
}

