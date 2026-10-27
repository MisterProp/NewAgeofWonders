// /AoWInterface.dpl : AoWPanel.TAOWPanel.SetILTexture@23EDC2EF

void AoWPanel_TAOWPanel_SetILTexture_23EDC2EF(int param_1,int param_2)

{
  undefined1 *puVar1;
  undefined4 *in_FS_OFFSET;
  UINT uType;
  undefined4 uStack_18;
  undefined1 *puStack_14;
  undefined1 *puStack_10;
  LPCSTR local_8;
  
                    /* 0x1f418  433  AoWPanel.TAOWPanel.SetILTexture@23EDC2EF */
  puStack_10 = &stack0xfffffffc;
  local_8 = (LPCSTR)0x0;
  puStack_14 = &LAB_0041f482;
  uStack_18 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_18;
  if (param_2 == 0) {
    *(undefined4 *)(param_1 + 0x1cc) = 0;
    puStack_10 = &stack0xfffffffc;
  }
  else if (*(int *)(param_2 + 0x44) == 1) {
    *(int *)(param_1 + 0x1cc) = param_2;
    puStack_10 = &stack0xfffffffc;
  }
  else {
    uType = 0;
    System__LStrCat3_51F89FF7(&local_8,"Error - ",*(undefined4 *)(param_1 + 8));
    MessageBoxA((HWND)0x0,"Textures must have 1 Image",local_8,uType);
  }
  puVar1 = puStack_10;
  *in_FS_OFFSET = uStack_18;
  puStack_10 = &LAB_0041f489;
  puStack_14 = (undefined1 *)0x41f481;
  System__LStrClr_40929B27(&local_8,uStack_18,puVar1);
  return;
}

