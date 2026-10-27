// /AoWEPACK.dpl : 5575cf4c

void AoWE_TProductionControl_CanExecuteProduction_23EDC2EF
               (int *param_1,int param_2,undefined4 param_3)

{
  undefined1 *puVar1;
  char cVar2;
  int iVar3;
  int iVar4;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_24;
  undefined1 *puStack_20;
  undefined1 *puStack_1c;
  undefined4 local_c;
  undefined4 local_8;
  
                    /* 0x5cf4c  2359  AoWE.TProductionControl.CanExecuteProduction@23EDC2EF */
  puStack_1c = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  puStack_20 = &LAB_5575d0ee;
  uStack_24 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_24;
  puVar1 = &stack0xfffffffc;
  if ((char)param_1[8] == '\x01') {
    System_LoadResString_02614E1B(&AoWE_WaitingRStr_8276FCC3,&local_8,param_3,&DAT_5575d108);
    AoWE_TranslateRStr_435799AA(local_8,&local_c);
    SysUtils_LowerCase_435799AA(local_c,&local_8);
    System__LStrCatN_51F89FF7(param_3,3);
    puVar1 = puStack_1c;
  }
  puStack_1c = puVar1;
  if (*(int *)(param_2 + 8) == 1) {
    iVar3 = (**(code **)(*param_1 + 0xc4))(param_1,*(undefined4 *)(param_2 + 0xc));
    cVar2 = (**(code **)(*param_1 + 0x7c))();
    iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),(int)cVar2);
    if (*(int *)(iVar4 + 0xc4) < iVar3) {
      System_LoadResString_02614E1B(&AoWE_NotEnoughGoldRStr_8276FCC3,&local_c);
      AoWE_TranslateRStr_435799AA(local_c,&local_8);
      System__LStrAsg_51F89FF7(param_3,local_8);
    }
    else {
      iVar3 = (**(code **)(*param_1 + 200))(param_1,*(undefined4 *)(param_2 + 0xc));
      cVar2 = (**(code **)(*param_1 + 0x7c))();
      iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),(int)cVar2);
      if (*(int *)(*(int *)(iVar4 + 0x54) + 0x24) < iVar3) {
        System_LoadResString_02614E1B(&AoWE_NotEnoughManaRStr_8276FCC3,&local_c);
        AoWE_TranslateRStr_435799AA(local_c,&local_8);
        System__LStrAsg_51F89FF7(param_3,local_8);
      }
    }
  }
  else if (*(int *)(param_2 + 8) == 2) {
    iVar3 = (**(code **)(*param_1 + 0xe4))(param_1,*(undefined4 *)(param_2 + 0xc));
    cVar2 = (**(code **)(*param_1 + 0x7c))();
    iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),(int)cVar2);
    if (*(int *)(iVar4 + 0xc4) < iVar3) {
      System_LoadResString_02614E1B(&AoWE_NotEnoughGoldRStr_8276FCC3,&local_c);
      AoWE_TranslateRStr_435799AA(local_c,&local_8);
      System__LStrAsg_51F89FF7(param_3,local_8);
    }
  }
  puVar1 = puStack_1c;
  *in_FS_OFFSET = uStack_24;
  puStack_1c = &LAB_5575d0f5;
  puStack_20 = (undefined1 *)0x5575d0ed;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar1);
  return;
}

