
void FUN_0043dccc(int param_1,undefined4 param_2)

{
  undefined1 *puVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_24;
  undefined1 *puStack_20;
  undefined1 *puStack_1c;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_1c = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  puStack_20 = &LAB_0043deb9;
  uStack_24 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_24;
  iVar2 = (**(code **)(**(int **)(*(int *)(*(int *)(param_1 + 0x2b0) + 0x50) + 0x14) + 0x54))();
  if ((iVar2 <= *(int *)(*(int *)(param_1 + 0x130) + 0x120)) || (*(char *)(param_1 + 0x2cc) == '\0')
     ) {
    if (*(char *)(param_1 + 0x2cc) == '\x02') {
      AoWListBox_TAOWListBox_SetIndex_23EDC2EF(param_2,*(undefined4 *)(param_1 + 0x2c8));
    }
    else {
      AoWListBox_TAOWListBox_SetIndex_23EDC2EF(param_2,0xffffffff);
    }
    goto LAB_0043de9e;
  }
  iVar2 = *(int *)(*(int *)(param_1 + 0x130) + 0x120);
  if (iVar2 == -1) {
    FUN_0043bb14(param_1,CONCAT31((int3)((uint)*(int *)(param_1 + 0x130) >> 8),1));
    FUN_0043c414(param_1);
    goto LAB_0043de9e;
  }
  *(int *)(param_1 + 0x2c8) = iVar2;
  iVar2 = AoWE_TProductionQueue_GetItem_23EDC2EF
                    (*(undefined4 *)(*(int *)(*(int *)(param_1 + 0x2b0) + 0x50) + 0x14),iVar2);
  uVar3 = City_ProductionIDToCityAction_CFDECB0A(*(undefined4 *)(iVar2 + 8));
  if ((char)uVar3 == '\b') {
    iVar2 = AoWE_TProductionQueue_GetItem_23EDC2EF
                      (*(undefined4 *)(*(int *)(*(int *)(param_1 + 0x2b0) + 0x50) + 0x14),
                       *(undefined4 *)(param_1 + 0x2c8));
    *(undefined4 *)(param_1 + 0x2c4) = *(undefined4 *)(iVar2 + 0xc);
    iVar2 = *(int *)(param_1 + 0x2c4);
    if (-1 < iVar2) {
      iVar4 = (**(code **)(**(int **)(*(int *)(*(int *)(param_1 + 0x2b0) + 0x50) + 0x1c) + 0x54))();
      if (iVar2 < iVar4) goto LAB_0043dd9e;
    }
    *(undefined4 *)(param_1 + 0x2c4) = 0;
  }
LAB_0043dd9e:
  FUN_0043c43c(param_1,uVar3);
  FUN_0043bb14(param_1,2);
  AoWListBox_TAOWListBox_SetIndex_23EDC2EF
            (*(undefined4 *)(param_1 + 0x130),*(undefined4 *)(param_1 + 0x2c8));
  (**(code **)(**(int **)(param_1 + 300) + 0x6c))(*(int **)(param_1 + 300),1);
  System_LoadResString_02614E1B(AoWE_DeleteRStr_04ECFCC5_exref,&local_c);
  AoWE_TranslateRStr_435799AA(local_c,&local_8);
  AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 300),local_8);
  if ((*(int *)(param_1 + 0x2c8) == 0) &&
     (*(char *)(*(int *)(*(int *)(param_1 + 0x2b0) + 0x50) + 0x20) == '\x01')) {
    (**(code **)(**(int **)(param_1 + 0x150) + 0x6c))(*(int **)(param_1 + 0x150),1);
    System_LoadResString_02614E1B(AoWE_ExecuteRStr_04ECFCC5_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_8);
    AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x150),local_8);
  }
  else {
    (**(code **)(**(int **)(param_1 + 0x150) + 0x6c))(*(int **)(param_1 + 0x150),0);
  }
  (**(code **)(**(int **)(param_1 + 0x2a8) + 0x6c))(*(int **)(param_1 + 0x2a8),0);
  (**(code **)(**(int **)(param_1 + 0x2a4) + 0x6c))(*(int **)(param_1 + 0x2a4),0);
LAB_0043de9e:
  puVar1 = puStack_1c;
  *in_FS_OFFSET = uStack_24;
  puStack_1c = &LAB_0043dec0;
  puStack_20 = (undefined1 *)0x43deb8;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar1);
  return;
}

