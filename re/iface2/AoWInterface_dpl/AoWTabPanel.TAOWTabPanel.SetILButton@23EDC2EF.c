// /AoWInterface.dpl : AoWTabPanel.TAOWTabPanel.SetILButton@23EDC2EF

void AoWTabPanel_TAOWTabPanel_SetILButton_23EDC2EF(int param_1,int param_2)

{
  undefined1 *puVar1;
  undefined4 *in_FS_OFFSET;
  UINT uType;
  undefined4 uStack_18;
  undefined1 *puStack_14;
  undefined1 *puStack_10;
  LPCSTR local_8;
  
                    /* 0x13f6c  318  AoWTabPanel.TAOWTabPanel.SetILButton@23EDC2EF */
  puStack_10 = &stack0xfffffffc;
  local_8 = (LPCSTR)0x0;
  puStack_14 = &LAB_00413fd6;
  uStack_18 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_18;
  if (param_2 == 0) {
    *(undefined4 *)(param_1 + 0x118) = 0;
    puStack_10 = &stack0xfffffffc;
  }
  else if (*(int *)(param_2 + 0x44) == 6) {
    *(int *)(param_1 + 0x118) = param_2;
    puStack_10 = &stack0xfffffffc;
  }
  else {
    uType = 0;
    System__LStrCat3_51F89FF7(&local_8,"Error - ",*(undefined4 *)(param_1 + 8));
    MessageBoxA((HWND)0x0,"TabPanels must have 6 Images",local_8,uType);
  }
  puVar1 = puStack_10;
  *in_FS_OFFSET = uStack_18;
  puStack_10 = &LAB_00413fdd;
  puStack_14 = (undefined1 *)0x413fd5;
  System__LStrClr_40929B27(&local_8,uStack_18,puVar1);
  return;
}

