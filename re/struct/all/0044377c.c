
void FUN_0044377c(int param_1,int *param_2)

{
  int *piVar1;
  undefined1 *puVar2;
  char cVar3;
  undefined4 uVar4;
  int iVar5;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_2c;
  undefined1 *puStack_28;
  undefined1 *puStack_24;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  int *local_8;
  
  puStack_24 = &stack0xfffffffc;
  local_8 = (int *)0x0;
  local_c = 0;
  local_10 = 0;
  local_14 = 0;
  puStack_28 = &LAB_00443bf2;
  uStack_2c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_2c;
  FUN_004425d8(param_1);
  FUN_0044275c(param_1,param_2);
  System_LoadResString_02614E1B(AoWE_EnterRStr_04ECFCC5_exref,&local_14);
  AoWE_TranslateRStr_435799AA(local_14,&local_10);
  AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x58),local_10);
  (**(code **)(**(int **)(param_1 + 0x58) + 0x6c))(*(int **)(param_1 + 0x58),1);
  iVar5 = *(int *)PTR_DAT_0045a420;
  *(undefined4 *)(iVar5 + 0xb4) = *(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0xa8);
  (**(code **)(**(int **)(param_1 + 0x68) + 0x6c))
            (*(int **)(param_1 + 0x68),CONCAT31((int3)((uint)iVar5 >> 8),1));
  *(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0xb8) =
       *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a420 + 0xb4) + 0x44);
  (**(code **)(**(int **)(param_1 + 0x154) + 0x6c))(*(int **)(param_1 + 0x154),0);
  (**(code **)(**(int **)(*(int *)(param_1 + 0x70) + 0x118) + 0x40))();
  (**(code **)(**(int **)(param_1 + 0x14c) + 0x6c))(*(int **)(param_1 + 0x14c),1);
  (**(code **)(**(int **)(param_1 + 0x138) + 0x6c))(*(int **)(param_1 + 0x138),1);
  (**(code **)(**(int **)(param_1 + 0x13c) + 0x6c))(*(int **)(param_1 + 0x13c),1);
  cVar3 = (**(code **)(*param_2 + 0x230))();
  if (cVar3 == '\b') {
    iVar5 = (**(code **)(*param_2 + 0x22c))();
    if (iVar5 == 0) {
      (**(code **)(**(int **)(param_1 + 0x14c) + 0x6c))(*(int **)(param_1 + 0x14c),0);
      (**(code **)(**(int **)(param_1 + 0x138) + 0x6c))(*(int **)(param_1 + 0x138),0);
      (**(code **)(**(int **)(param_1 + 0x13c) + 0x6c))(*(int **)(param_1 + 0x13c),0);
    }
    else {
      AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(param_1 + 0x138),0x1f);
      uVar4 = (**(code **)(*param_2 + 500))();
      SysUtils_IntToStr_0F6FDFF6(uVar4,&local_10);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x13c),local_10);
    }
  }
  else {
    AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(param_1 + 0x138),0x21);
    uVar4 = (**(code **)(*param_2 + 0x1f8))();
    SysUtils_IntToStr_0F6FDFF6(uVar4,&local_10);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x13c),local_10);
  }
  (**(code **)(**(int **)(param_1 + 0x68) + 0x6c))(*(int **)(param_1 + 0x68),1);
  (**(code **)(*param_2 + 0x148))(param_2,&local_10);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 100),local_10);
  iVar5 = (**(code **)(**(int **)(param_2[0x11] + 0x14) + 0x54))();
  if (iVar5 == 0) {
    cVar3 = (**(code **)(*param_2 + 0x230))();
    if (cVar3 == '\b') {
      piVar1 = *(int **)(*(int *)(param_1 + 0x70) + 0x118);
      (**(code **)(*piVar1 + 0x34))(piVar1,0);
      iVar5 = (**(code **)(*param_2 + 0x22c))();
      if (iVar5 == 0) {
        System_LoadResString_02614E1B(AoWE_NoneRStr_8276FCC3_exref,&local_14);
        AoWE_TranslateRStr_435799AA(local_14,&local_10);
        piVar1 = *(int **)(*(int *)(param_1 + 0x70) + 0x118);
        (**(code **)(*piVar1 + 0x34))(piVar1,local_10);
      }
      else {
        System_LoadResString_02614E1B(AoWE_ProducingMerchandiseRStr_04ECFCC5_exref,&local_14);
        AoWE_TranslateRStr_435799AA(local_14,&local_10);
        piVar1 = *(int **)(*(int *)(param_1 + 0x70) + 0x118);
        (**(code **)(*piVar1 + 0x34))(piVar1,local_10);
      }
    }
    else {
      System_LoadResString_02614E1B(AoWE_GeneratingExtraPowerRStr_04ECFCC5_exref,&local_14);
      AoWE_TranslateRStr_435799AA(local_14,&local_10);
      piVar1 = *(int **)(*(int *)(param_1 + 0x70) + 0x118);
      (**(code **)(*piVar1 + 0x34))(piVar1,local_10);
    }
  }
  else {
    iVar5 = AoWE_TProductionQueue_GetItem_23EDC2EF(*(undefined4 *)(param_2[0x11] + 0x14),0);
    if (*(int *)(iVar5 + 8) == 1) {
      piVar1 = (int *)param_2[0x11];
      iVar5 = AoWE_TProductionQueue_GetItem_23EDC2EF(piVar1[5],0);
      local_8 = (int *)(**(code **)(*piVar1 + 0x9c))(piVar1,*(undefined4 *)(iVar5 + 0xc));
      (**(code **)(**(int **)(*(int *)(param_1 + 0x70) + 0x118) + 0x40))();
      System_LoadResString_02614E1B(AoWE_ProducingRStr_04ECFCC5_exref,&local_14);
      AoWE_TranslateRStr_435799AA(local_14,&local_10);
      piVar1 = *(int **)(*(int *)(param_1 + 0x70) + 0x118);
      (**(code **)(*piVar1 + 0x34))(piVar1,local_10);
      (**(code **)(*local_8 + 0xf8))(local_8,&local_10);
      piVar1 = *(int **)(*(int *)(param_1 + 0x70) + 0x118);
      (**(code **)(*piVar1 + 0x34))(piVar1,local_10);
      (**(code **)(*local_8 + 0x2c))();
    }
    else if (*(int *)(iVar5 + 8) == 2) {
      piVar1 = (int *)param_2[0x11];
      iVar5 = AoWE_TProductionQueue_GetItem_23EDC2EF(piVar1[5],0);
      local_8 = (int *)(**(code **)(*piVar1 + 0x9c))(piVar1,*(undefined4 *)(iVar5 + 0xc));
      System_LoadResString_02614E1B(AoWE_InstallingRStr_04ECFCC5_exref,&local_14);
      AoWE_TranslateRStr_435799AA(local_14,&local_10);
      piVar1 = *(int **)(*(int *)(param_1 + 0x70) + 0x118);
      (**(code **)(*piVar1 + 0x34))(piVar1,local_10);
      (**(code **)(*local_8 + 0xf8))(local_8,&local_10);
      piVar1 = *(int **)(*(int *)(param_1 + 0x70) + 0x118);
      (**(code **)(*piVar1 + 0x34))(piVar1,local_10);
      (**(code **)(*local_8 + 0x2c))();
    }
    else {
      uVar4 = AoWE_TProductionQueue_GetItem_23EDC2EF(*(undefined4 *)(param_2[0x11] + 0x14),0);
      AoWE_TProductionItem_GetNameProgressive_23EDC2EF(uVar4,&local_10);
      piVar1 = *(int **)(*(int *)(param_1 + 0x70) + 0x118);
      (**(code **)(*piVar1 + 0x2c))(piVar1,local_10);
    }
    piVar1 = (int *)param_2[0x11];
    if ((char)piVar1[8] == '\x02') {
      if (*(char *)(piVar1[9] + 0x10) == -1) {
        System_LoadResString_02614E1B(AoWE_UnlimitedRStr_04ECFCC5_exref,&local_10);
        AoWE_TranslateRStr_435799AA(local_10,&local_14);
        SysUtils_LowerCase_435799AA(local_14,&local_10);
        piVar1 = *(int **)(*(int *)(param_1 + 0x140) + 0x118);
        (**(code **)(*piVar1 + 0x34))(piVar1,local_10);
      }
      else {
        AoWE_FmtTurnsLeftStr_0F6FDFF6
                  ((int)*(char *)(*(int *)(param_2[0x11] + 0x24) + 0x10),&local_10);
        piVar1 = *(int **)(*(int *)(param_1 + 0x70) + 0x118);
        (**(code **)(*piVar1 + 0x34))(piVar1,local_10);
      }
    }
    else if ((char)piVar1[8] == '\x01') {
      uVar4 = AoWE_TProductionQueue_GetItem_23EDC2EF(piVar1[5],0);
      (**(code **)(*piVar1 + 0xb0))(piVar1,uVar4,&local_c);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x154),local_c);
      (**(code **)(**(int **)(param_1 + 0x154) + 0x6c))(*(int **)(param_1 + 0x154),1);
    }
  }
  (**(code **)(**(int **)(param_1 + 0x10c) + 0x6c))(*(int **)(param_1 + 0x10c),1);
  puVar2 = puStack_24;
  *in_FS_OFFSET = uStack_2c;
  puStack_24 = &LAB_00443bf9;
  puStack_28 = (undefined1 *)0x443bf1;
  System__LStrArrayClr_51F89FF7(&local_14,3,puVar2);
  return;
}

