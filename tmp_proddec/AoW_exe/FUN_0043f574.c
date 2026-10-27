// /AoW.exe : FUN_0043f574

void FUN_0043f574(int param_1)

{
  undefined1 *puVar1;
  char cVar2;
  undefined4 uVar3;
  int iVar4;
  int *piVar5;
  int iVar6;
  undefined4 *in_FS_OFFSET;
  undefined4 *puVar7;
  undefined *puVar8;
  undefined4 *puVar9;
  undefined4 uVar10;
  undefined4 uStack_3c;
  undefined1 *puStack_38;
  undefined1 *puStack_34;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  int local_c;
  undefined4 local_8;
  
  puStack_34 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  local_10 = 0;
  local_14 = 0;
  local_18 = 0;
  local_1c = 0;
  local_20 = 0;
  local_24 = 0;
  puStack_38 = &LAB_0043fd0e;
  uStack_3c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_3c;
  (**(code **)(**(int **)(*(int *)(param_1 + 0x140) + 0x118) + 0x40))();
  (**(code **)(**(int **)(param_1 + 0x1e8) + 0x148))(*(int **)(param_1 + 0x1e8),&local_10);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x9c),local_10);
  System_LoadResString_02614E1B(AoWE_IncomeRStr_04ECFCC5_exref,&local_14);
  AoWE_TranslateRStr_435799AA(local_14,&local_10);
  piVar5 = *(int **)(*(int *)(param_1 + 0x140) + 0x118);
  (**(code **)(*piVar5 + 0x34))(piVar5,local_10);
  cVar2 = (**(code **)(**(int **)(param_1 + 0x1e8) + 0x230))();
  if (cVar2 != '\b') {
    puVar9 = &local_10;
    uVar3 = (**(code **)(**(int **)(param_1 + 0x1e8) + 0x1f8))();
    SysUtils_IntToStr_0F6FDFF6(uVar3,&local_14);
    local_1c = local_14;
    local_18 = CONCAT31(local_18._1_3_,0xb);
    puVar7 = &local_1c;
    System_LoadResString_02614E1B(AoWE_XPowerRStr_04ECFCC5_exref,&local_24);
    AoWE_TranslateRStr_435799AA(local_24,&local_10);
    System__LStrCat3_51F89FF7(&local_20,&DAT_0043fd24,local_10,puVar7,puVar9);
    AoWE_Format_D8F48273(local_20,puVar7,0);
    piVar5 = *(int **)(*(int *)(param_1 + 0x140) + 0x118);
    (**(code **)(*piVar5 + 0x34))(piVar5,local_10);
  }
  iVar4 = (**(code **)(**(int **)(param_1 + 0x1e8) + 500))();
  if (iVar4 != 0) {
    puVar9 = &local_10;
    uVar3 = (**(code **)(**(int **)(param_1 + 0x1e8) + 500))();
    SysUtils_IntToStr_0F6FDFF6(uVar3,&local_14);
    local_1c = local_14;
    local_18 = CONCAT31(local_18._1_3_,0xb);
    puVar7 = &local_1c;
    System_LoadResString_02614E1B(AoWE_XGoldRStr_04ECFCC5_exref,&local_24);
    AoWE_TranslateRStr_435799AA(local_24,&local_10);
    System__LStrCat3_51F89FF7(&local_20,&DAT_0043fd24,local_10,puVar7,puVar9);
    AoWE_Format_D8F48273(local_20,puVar7,0);
    piVar5 = *(int **)(*(int *)(param_1 + 0x140) + 0x118);
    (**(code **)(*piVar5 + 0x34))(piVar5,local_10);
  }
  cVar2 = (**(code **)(**(int **)(param_1 + 0x1e8) + 0x230))();
  if (cVar2 == '\b') {
    cVar2 = System__IsClass_51F89FF7
                      (*(undefined4 *)(param_1 + 0x1e8),Shipyard__TShipyard_26B796A3_exref);
    if (cVar2 == '\0') {
      AoWFrame_TAoWFrame_SetILTIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x138),0x41);
    }
    else {
      (**(code **)(**(int **)(*(int *)(param_1 + 0x140) + 0x118) + 0x40))();
      AoWFrame_TAoWFrame_SetILTIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x138),0x44);
    }
    (**(code **)(**(int **)(param_1 + 300) + 0x9c))
              (*(int **)(param_1 + 300),*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0xec));
    cVar2 = System__IsClass_51F89FF7
                      (*(undefined4 *)(param_1 + 0x1e8),Shipyard__TShipyard_26B796A3_exref);
    if (cVar2 == '\0') {
      AoWFrame_TAoWFrame_SetILTIndex_23EDC2EF(*(undefined4 *)(param_1 + 300),0x42);
    }
    else {
      AoWFrame_TAoWFrame_SetILTIndex_23EDC2EF(*(undefined4 *)(param_1 + 300),0x43);
    }
    (**(code **)(**(int **)(param_1 + 0x1cc) + 0x6c))(*(int **)(param_1 + 0x1cc),0);
  }
  else {
    AoWFrame_TAoWFrame_SetILTIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x138),0x40);
    (**(code **)(**(int **)(param_1 + 300) + 0x9c))(*(int **)(param_1 + 300),0);
    (**(code **)(**(int **)(param_1 + 0x1e8) + 0x230))();
    uVar3 = FUN_0043f184();
    (**(code **)(**(int **)(param_1 + 0x1cc) + 0xa0))(*(int **)(param_1 + 0x1cc),uVar3);
    (**(code **)(**(int **)(param_1 + 0x1cc) + 0x6c))(*(int **)(param_1 + 0x1cc),1);
  }
  (**(code **)(**(int **)(*(int *)(param_1 + 0x13c) + 0x118) + 0x40))();
  (**(code **)(**(int **)(*(int *)(param_1 + 0x1c0) + 0x118) + 0x40))();
  (**(code **)(**(int **)(param_1 + 0x1e8) + 0x23c))
            (*(int **)(param_1 + 0x1e8),*(undefined4 *)(*(int *)(param_1 + 0x13c) + 0x118),
             *(undefined4 *)(*(int *)(param_1 + 0x1c0) + 0x118));
  (**(code **)(**(int **)(*(int *)(param_1 + 0x100) + 0x114) + 0x40))();
  iVar4 = (**(code **)(**(int **)(*(int *)(*(int *)(param_1 + 0x1e8) + 0x44) + 0x14) + 0x54))();
  if (-1 < iVar4 + -1) {
    iVar6 = 0;
    local_c = iVar4;
    do {
      SysUtils_IntToStr_0F6FDFF6(iVar6 + 1,&local_14);
      puVar8 = &DAT_0043fd30;
      uVar3 = local_14;
      piVar5 = (int *)AoWE_TProductionQueue_GetItem_23EDC2EF
                                (*(undefined4 *)(*(int *)(*(int *)(param_1 + 0x1e8) + 0x44) + 0x14),
                                 iVar6);
      (**(code **)(*piVar5 + 0x4c))(piVar5,&local_20,*piVar5,puVar8,uVar3);
      uVar3 = local_20;
      System__LStrCatN_51F89FF7(&local_10,3);
      piVar5 = *(int **)(*(int *)(param_1 + 0x100) + 0x114);
      iVar4 = *piVar5;
      (**(code **)(iVar4 + 0x34))(piVar5,local_10,iVar4,uVar3);
      iVar6 = iVar6 + 1;
      local_c = local_c + -1;
    } while (local_c != 0);
  }
  iVar4 = (**(code **)(**(int **)(*(int *)(*(int *)(param_1 + 0x1e8) + 0x44) + 0x14) + 0x54))();
  if (iVar4 < 5) {
    do {
      SysUtils_IntToStr_0F6FDFF6(iVar4 + 1,&local_14);
      puVar8 = &DAT_0043fd30;
      uVar10 = local_14;
      System_LoadResString_02614E1B(AoWE_EmptyQueueRStr_04ECFCC5_exref,&local_24);
      AoWE_TranslateRStr_435799AA(local_24,&local_20);
      uVar3 = local_20;
      System__LStrCatN_51F89FF7(&local_10,3);
      piVar5 = *(int **)(*(int *)(param_1 + 0x100) + 0x114);
      iVar6 = *piVar5;
      (**(code **)(iVar6 + 0x34))(piVar5,local_10,iVar6,uVar3,puVar8,uVar10);
      iVar4 = iVar4 + 1;
    } while (iVar4 != 5);
  }
  iVar4 = (**(code **)(**(int **)(*(int *)(*(int *)(param_1 + 0x1e8) + 0x44) + 0x14) + 0x54))();
  if (iVar4 <= *(int *)(*(int *)(param_1 + 0x100) + 0x120)) {
    iVar4 = (**(code **)(**(int **)(*(int *)(*(int *)(param_1 + 0x1e8) + 0x44) + 0x14) + 0x54))();
    AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x100),iVar4 + -1);
    *(undefined4 *)(param_1 + 0x1f0) = *(undefined4 *)(*(int *)(param_1 + 0x100) + 0x120);
  }
  (**(code **)(**(int **)(param_1 + 0x158) + 0xac))
            (*(int **)(param_1 + 0x158),
             *(undefined1 *)(*(int *)(*(int *)(param_1 + 0x1e8) + 0x44) + 0x18));
  piVar5 = *(int **)(param_1 + 0x1e8);
  cVar2 = *(char *)(piVar5[0x11] + 0x20);
  if (cVar2 == '\x02') {
    uVar3 = AoWE_TProductionQueue_GetItem_23EDC2EF(*(undefined4 *)(piVar5[0x11] + 0x14),0);
    AoWE_TProductionItem_GetNameProgressive_23EDC2EF(uVar3,&local_10);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x10c),local_10);
    if (*(char *)(*(int *)(*(int *)(*(int *)(param_1 + 0x1e8) + 0x44) + 0x24) + 0x10) == -1) {
      (**(code **)(**(int **)(param_1 + 0x110) + 0x6c))(*(int **)(param_1 + 0x110),0);
      (**(code **)(**(int **)(param_1 + 0x11c) + 0x6c))(*(int **)(param_1 + 0x11c),0);
    }
    else {
      AoWE_FmtTurnsLeftStr_0F6FDFF6
                ((int)*(char *)(*(int *)(*(int *)(*(int *)(param_1 + 0x1e8) + 0x44) + 0x24) + 0x10),
                 &local_10);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x110),local_10);
      (**(code **)(**(int **)(param_1 + 0x110) + 0x6c))(*(int **)(param_1 + 0x110),1);
      (**(code **)(**(int **)(param_1 + 0x110) + 0x70))(*(int **)(param_1 + 0x110),0x678376);
      iVar4 = *(int *)(param_1 + 0x11c);
      *(undefined4 *)(iVar4 + 0x114) = 0;
      *(int *)(iVar4 + 0x118) =
           (int)*(char *)(*(int *)(*(int *)(*(int *)(param_1 + 0x1e8) + 0x44) + 0x24) + 0x1c);
      iVar6 = *(int *)(*(int *)(*(int *)(param_1 + 0x1e8) + 0x44) + 0x24);
      AOWGauge_TAoWGauge_SetPos_23EDC2EF
                (iVar4,(int)*(char *)(iVar6 + 0x1c) - (int)*(char *)(iVar6 + 0x10));
      (**(code **)(**(int **)(param_1 + 0x11c) + 0x6c))(*(int **)(param_1 + 0x11c),1);
    }
    uVar3 = AoWE_TProductionQueue_GetItem_23EDC2EF
                      (*(undefined4 *)(*(int *)(*(int *)(param_1 + 0x1e8) + 0x44) + 0x14),0);
    uVar3 = FUN_0043f534(param_1,uVar3);
    AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(param_1 + 0x164),uVar3);
  }
  else if (cVar2 == '\0') {
    (**(code **)(*piVar5 + 0x1b4))(piVar5,&local_10);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x10c),local_10);
    System_LoadResString_02614E1B(AoWE_DefaultRStr_04ECFCC5_exref,&local_14);
    AoWE_TranslateRStr_435799AA(local_14,&local_20);
    SysUtils_LowerCase_435799AA(local_20,&local_14);
    puVar8 = &DAT_0043fd48;
    uVar3 = local_14;
    System__LStrCatN_51F89FF7(&local_10,3);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x110),local_10);
    iVar4 = **(int **)(param_1 + 0x110);
    (**(code **)(iVar4 + 0x70))(*(int **)(param_1 + 0x110),0x678376,iVar4,puVar8,uVar3);
    System_LoadResString_02614E1B(AoWE_NoneRStr_8276FCC3_exref,&local_14);
    AoWE_TranslateRStr_435799AA(local_14,&local_10);
    iVar4 = SysUtils_CompareText_8779A639
                      (*(undefined4 *)(*(int *)(param_1 + 0x10c) + 0x90),local_10);
    if (iVar4 == 0) {
      (**(code **)(**(int **)(param_1 + 0x110) + 0x6c))(*(int **)(param_1 + 0x110),0);
      AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(param_1 + 0x164),0x3e);
    }
    else {
      (**(code **)(**(int **)(param_1 + 0x110) + 0x6c))(*(int **)(param_1 + 0x110),1);
      iVar4 = Engine_TEObject_Create_23EDC2EF(AoWE__TProductionItem_E3617239_exref,1,0);
      *(undefined4 *)(iVar4 + 8) = 10;
      uVar3 = FUN_0043f534(param_1,iVar4);
      AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(param_1 + 0x164),uVar3);
      System_TObject_Free_23EDC2EF(iVar4);
    }
    (**(code **)(**(int **)(param_1 + 0x11c) + 0x6c))(*(int **)(param_1 + 0x11c),0);
  }
  else if (cVar2 == '\x01') {
    System__LStrClr_40929B27(&local_8);
    uVar3 = AoWE_TProductionQueue_GetItem_23EDC2EF
                      (*(undefined4 *)(*(int *)(*(int *)(param_1 + 0x1e8) + 0x44) + 0x14),0);
    AoWE_TProductionItem_GetNameProgressive_23EDC2EF(uVar3,&local_10);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x10c),local_10);
    piVar5 = *(int **)(*(int *)(param_1 + 0x1e8) + 0x44);
    uVar3 = AoWE_TProductionQueue_GetItem_23EDC2EF(piVar5[5],0);
    (**(code **)(*piVar5 + 0xb0))(piVar5,uVar3,&local_8);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x110),local_8);
    (**(code **)(**(int **)(param_1 + 0x110) + 0x70))(*(int **)(param_1 + 0x110),0xff);
    (**(code **)(**(int **)(param_1 + 0x110) + 0x6c))(*(int **)(param_1 + 0x110),1);
    (**(code **)(**(int **)(param_1 + 0x11c) + 0x6c))(*(int **)(param_1 + 0x11c),0);
    uVar3 = AoWE_TProductionQueue_GetItem_23EDC2EF
                      (*(undefined4 *)(*(int *)(*(int *)(param_1 + 0x1e8) + 0x44) + 0x14),0);
    uVar3 = FUN_0043f534(param_1,uVar3);
    AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(param_1 + 0x164),uVar3);
  }
  if (*(char *)(param_1 + 500) == '\x01') {
    if (*(int *)(param_1 + 0x1ec) != -1) {
      FUN_0043ffa8(param_1,*(int *)(param_1 + 0x1ec));
    }
  }
  else if (*(char *)(param_1 + 500) == '\x02') {
    FUN_00441020(param_1,param_1);
  }
  puVar1 = puStack_34;
  *(undefined1 *)(*(int *)(param_1 + 0x44) + 0xa4) = 0;
  *in_FS_OFFSET = uStack_3c;
  puStack_34 = &LAB_0043fd15;
  puStack_38 = (undefined1 *)0x43fcf8;
  System__LStrArrayClr_51F89FF7(&local_24,2,puVar1);
  puStack_38 = (undefined1 *)0x43fd05;
  System__LStrArrayClr_51F89FF7(&local_14,2);
  puStack_38 = (undefined1 *)0x43fd0d;
  System__LStrClr_40929B27(&local_8);
  return;
}

