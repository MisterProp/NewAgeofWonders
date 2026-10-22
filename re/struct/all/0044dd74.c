
void FUN_0044dd74(int param_1)

{
  int *piVar1;
  undefined1 *puVar2;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_14 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  puStack_18 = &LAB_0044de05;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  puVar2 = &stack0xfffffffc;
  if (*(char *)(param_1 + 0xb0) != '\0') {
    if (*(char *)(*(int *)(param_1 + 0xa8) + 0x28) == '\0') {
      piVar1 = *(int **)(param_1 + 0x70);
      puVar2 = &stack0xfffffffc;
      if (*(char *)((int)piVar1 + 0x75) == '\0') {
        (**(code **)(*piVar1 + 0x6c))(piVar1,1);
        (**(code **)(**(int **)(param_1 + 0x6c) + 0x6c))(*(int **)(param_1 + 0x6c),0);
        puVar2 = puStack_14;
      }
    }
    else {
      puStack_14 = &stack0xfffffffc;
      System_LoadResString_02614E1B(AoWE_GameSuccessfullySentRStr_04ECFCC5_exref,&local_c);
      AoWE_TranslateRStr_435799AA(local_c,&local_8);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x60),local_8);
      (**(code **)(**(int **)(param_1 + 0x5c) + 0x6c))(*(int **)(param_1 + 0x5c),1);
      puVar2 = puStack_14;
    }
  }
  puStack_14 = puVar2;
  puVar2 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_0044de0c;
  puStack_18 = (undefined1 *)0x44de04;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar2);
  return;
}

