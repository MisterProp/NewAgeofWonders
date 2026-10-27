// /AoWEPACK.dpl : 5575c7a0

void AoWE_TProductionItem_GetNameProgressive_23EDC2EF(int param_1,undefined4 param_2)

{
  int *piVar1;
  undefined1 *puVar2;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_8;
  
                    /* 0x5c7a0  2388  AoWE.TProductionItem.GetNameProgressive@23EDC2EF */
  puStack_14 = &stack0xfffffffc;
  local_8 = 0;
  puStack_18 = &LAB_5575c800;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  if (*(int *)(param_1 + 4) == 0) {
    System_LoadResString_02614E1B(&AoWE_UnknownRStr_8276FCC3,&local_8);
    AoWE_TranslateRStr_435799AA(local_8,param_2);
  }
  else {
    piVar1 = *(int **)(*(int *)(param_1 + 4) + 0xc);
    puStack_14 = &stack0xfffffffc;
    (**(code **)(*piVar1 + 0xc0))(piVar1,param_1,param_2);
  }
  puVar2 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_5575c807;
  puStack_18 = (undefined1 *)0x5575c7ff;
  System__LStrClr_40929B27(&local_8,uStack_1c,puVar2);
  return;
}

