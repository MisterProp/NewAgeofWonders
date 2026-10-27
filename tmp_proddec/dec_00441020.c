
void FUN_00441020(int param_1)

{
  undefined1 *puVar1;
  int iVar2;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_20;
  undefined1 *puStack_1c;
  undefined1 *puStack_18;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_18 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  puStack_1c = &LAB_004411e8;
  uStack_20 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_20;
  iVar2 = (**(code **)(**(int **)(*(int *)(*(int *)(param_1 + 0x1e8) + 0x44) + 0x14) + 0x54))();
  if ((*(int *)(*(int *)(param_1 + 0x100) + 0x120) < iVar2) && (*(char *)(param_1 + 500) != '\0')) {
    iVar2 = *(int *)(*(int *)(param_1 + 0x100) + 0x120);
    if (iVar2 == -1) {
      FUN_0043fd4c(param_1,CONCAT31((int3)((uint)*(int *)(param_1 + 0x100) >> 8),1));
      FUN_0043f50c(param_1);
    }
    else {
      *(int *)(param_1 + 0x1f0) = iVar2;
      iVar2 = AoWE_TProductionQueue_GetItem_23EDC2EF
                        (*(undefined4 *)(*(int *)(*(int *)(param_1 + 0x1e8) + 0x44) + 0x14),iVar2);
      if (*(int *)(iVar2 + 8) == 1) {
        iVar2 = AoWE_TProductionQueue_GetItem_23EDC2EF
                          (*(undefined4 *)(*(int *)(*(int *)(param_1 + 0x1e8) + 0x44) + 0x14),
                           *(undefined4 *)(param_1 + 0x1f0));
        FUN_0043ffa8(param_1,*(undefined4 *)(iVar2 + 0xc));
      }
      else {
        FUN_0043ffa8(param_1,4);
      }
      FUN_0043fd4c(param_1,2);
      AoWListBox_TAOWListBox_SetIndex_23EDC2EF
                (*(undefined4 *)(param_1 + 0x100),*(undefined4 *)(param_1 + 0x1f0));
      (**(code **)(**(int **)(param_1 + 0xfc) + 0x6c))(*(int **)(param_1 + 0xfc),1);
      System_LoadResString_02614E1B(AoWE_DeleteRStr_04ECFCC5_exref,&local_c);
      AoWE_TranslateRStr_435799AA(local_c,&local_8);
      AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xfc),local_8);
      if ((*(int *)(param_1 + 0x1f0) == 0) &&
         (*(char *)(*(int *)(*(int *)(param_1 + 0x1e8) + 0x44) + 0x20) == '\x01')) {
        (**(code **)(**(int **)(param_1 + 0x120) + 0x6c))(*(int **)(param_1 + 0x120),1);
        System_LoadResString_02614E1B(AoWE_ExecuteRStr_04ECFCC5_exref,&local_c);
        AoWE_TranslateRStr_435799AA(local_c,&local_8);
        AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x120),local_8);
      }
      else {
        (**(code **)(**(int **)(param_1 + 0x120) + 0x6c))(*(int **)(param_1 + 0x120),0);
      }
      (**(code **)(**(int **)(param_1 + 0x1d8) + 0x6c))(*(int **)(param_1 + 0x1d8),0);
      (**(code **)(**(int **)(param_1 + 0x1d4) + 0x6c))(*(int **)(param_1 + 0x1d4),0);
    }
  }
  else if (*(char *)(param_1 + 500) == '\x02') {
    AoWListBox_TAOWListBox_SetIndex_23EDC2EF
              (*(undefined4 *)(param_1 + 0x100),*(undefined4 *)(param_1 + 0x1f0));
  }
  else {
    AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x100),0xffffffff);
  }
  puVar1 = puStack_18;
  *in_FS_OFFSET = uStack_20;
  puStack_18 = &LAB_004411ef;
  puStack_1c = (undefined1 *)0x4411e7;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar1);
  return;
}

