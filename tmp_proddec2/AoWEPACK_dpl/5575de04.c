// /AoWEPACK.dpl : 5575de04

void AoWE_TProductionControl_GetProductionName_23EDC2EF(int *param_1,int param_2,undefined4 param_3)

{
  undefined1 *puVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 *in_FS_OFFSET;
  undefined4 uVar5;
  undefined4 uStack_34;
  undefined1 *puStack_30;
  undefined1 *puStack_2c;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined1 local_c;
  undefined4 local_8;
  
                    /* 0x5de04  2323  AoWE.TProductionControl.GetProductionName@23EDC2EF */
  puStack_2c = &stack0xfffffffc;
  local_14 = 0;
  local_18 = 0;
  local_1c = 0;
  puStack_30 = &LAB_5575dfd3;
  uStack_34 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_34;
  local_8 = param_3;
  if (*(int *)(param_2 + 8) == 1) {
    iVar3 = AoWE_TUnitProductionList_GetItem_23EDC2EF(param_1[7],*(undefined4 *)(param_2 + 0xc));
    if (iVar3 == 0) {
      System_LoadResString_02614E1B(&AoWE_UnknownRStr_8276FCC3,&local_14);
      AoWE_TranslateRStr_435799AA(local_14,local_8);
    }
    else {
      iVar2 = (**(code **)(*param_1 + 0xc4))(param_1,*(undefined4 *)(param_2 + 0xc));
      if (iVar2 == 0) {
        uVar5 = local_8;
        uVar4 = AoWE_TUnitResourceList_GetUnitResource_23EDC2EF
                          (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x5c),*(undefined4 *)(iVar3 + 8)
                          );
        AoWE_TUnitResource_GetUnitName_23EDC2EF(uVar4,&local_14);
        local_10 = local_14;
        local_c = 0xb;
        System_LoadResString_02614E1B(&AoWE_SummonXRStr_8276FCC3,&local_1c);
        AoWE_TranslateRStr_435799AA(local_1c,&local_18);
        AoWE_Format_D8F48273(local_18,&local_10,0,uVar5);
      }
      else {
        uVar5 = local_8;
        uVar4 = AoWE_TUnitResourceList_GetUnitResource_23EDC2EF
                          (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x5c),*(undefined4 *)(iVar3 + 8)
                          );
        AoWE_TUnitResource_GetUnitName_23EDC2EF(uVar4,&local_14);
        local_10 = local_14;
        local_c = 0xb;
        System_LoadResString_02614E1B(&AoWE_ProduceXRStr_8276FCC3,&local_1c);
        AoWE_TranslateRStr_435799AA(local_1c,&local_18);
        AoWE_Format_D8F48273(local_18,&local_10,0,uVar5);
      }
    }
  }
  else if (*(int *)(param_2 + 8) == 2) {
    puStack_2c = &stack0xfffffffc;
    iVar3 = AoWE_TUnitProductionList_GetItem_23EDC2EF(param_1[7],*(undefined4 *)(param_2 + 0xc));
    if (iVar3 == 0) {
      System_LoadResString_02614E1B(&AoWE_UnknownRStr_8276FCC3,&local_14);
      AoWE_TranslateRStr_435799AA(local_14,local_8);
    }
    else {
      uVar5 = local_8;
      uVar4 = AoWE_TUnitResourceList_GetUnitResource_23EDC2EF
                        (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x5c),*(undefined4 *)(iVar3 + 8));
      AoWE_TUnitResource_GetUnitName_23EDC2EF(uVar4,&local_14);
      local_10 = local_14;
      local_c = 0xb;
      System_LoadResString_02614E1B(&AoWE_InstallXRStr_8276FCC3,&local_1c);
      AoWE_TranslateRStr_435799AA(local_1c,&local_18);
      AoWE_Format_D8F48273(local_18,&local_10,0,uVar5);
    }
  }
  else {
    puStack_2c = &stack0xfffffffc;
    System_LoadResString_02614E1B(&AoWE_UnknownRStr_8276FCC3,&local_14);
    AoWE_TranslateRStr_435799AA(local_14,local_8);
  }
  puVar1 = puStack_2c;
  *in_FS_OFFSET = uStack_34;
  puStack_2c = &LAB_5575dfda;
  puStack_30 = (undefined1 *)0x5575dfd2;
  System__LStrArrayClr_51F89FF7(&local_1c,3,puVar1);
  return;
}

