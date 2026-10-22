
void FUN_0041e01c(int param_1,int param_2,undefined4 param_3,undefined4 param_4,int param_5)

{
  int *piVar1;
  undefined1 *puVar2;
  int iVar3;
  undefined4 *in_FS_OFFSET;
  undefined1 uVar4;
  undefined4 uStack_24;
  undefined1 *puStack_20;
  undefined1 *puStack_1c;
  undefined4 local_c;
  int local_8;
  
  puStack_1c = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  puStack_20 = &LAB_0041e106;
  uStack_24 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_24;
  uVar4 = param_5 == 0x14;
  puVar2 = &stack0xfffffffc;
  if (0x14 < param_5) {
    iVar3 = FUN_0041bbb8(param_1);
    uVar4 = 0;
    puVar2 = puStack_1c;
    if (*(int *)(iVar3 + 0x38) == 1) {
      uVar4 = param_2 == *(int *)(param_1 + 0x5c);
      if ((bool)uVar4) {
        System_LoadResString_02614E1B(PTR_PTR_0045a200,&local_c);
        AoWE_TranslateRStr_435799AA(local_c,&local_8);
        puVar2 = puStack_1c;
      }
      else {
        iVar3 = FUN_0041bbb8(param_1);
        uVar4 = *(int *)(iVar3 + 0x34) == 0;
        if ((bool)uVar4) {
          System_LoadResString_02614E1B(PTR_PTR_0045a12c,&local_c);
          AoWE_TranslateRStr_435799AA(local_c,&local_8);
          puVar2 = puStack_1c;
        }
        else {
          System_LoadResString_02614E1B(PTR_PTR_0045a5d8,&local_c);
          AoWE_TranslateRStr_435799AA(local_c,&local_8);
          puVar2 = puStack_1c;
        }
      }
    }
  }
  puStack_1c = puVar2;
  iVar3 = *(int *)(param_1 + 100);
  System__LStrCmp_51F89FF7(*(undefined4 *)(iVar3 + 0x90),local_8);
  if ((!(bool)uVar4) && (local_8 != 0)) {
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(iVar3,local_8);
    piVar1 = *(int **)(param_1 + 100);
    (**(code **)(*piVar1 + 0x5c))(piVar1,0x1cc - piVar1[0x20]);
  }
  puVar2 = puStack_1c;
  *(undefined1 *)(param_1 + 0x158) = 1;
  *(undefined4 *)(param_2 + 0x78) = 0;
  *in_FS_OFFSET = uStack_24;
  puStack_1c = &LAB_0041e10d;
  puStack_20 = (undefined1 *)0x41e105;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar2);
  return;
}

