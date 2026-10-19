// /aowInt.dpl : 0x598038d8

void AOWInterfaceImgLib_TInterfaceIL_CheckIL_23EDC2EF
               (int param_1,int param_2,int param_3,int param_4)

{
  undefined1 *puVar1;
  char cVar2;
  int iVar3;
  undefined4 *in_FS_OFFSET;
  LPCSTR pCVar4;
  UINT UVar5;
  undefined4 uStack_2c;
  undefined1 *puStack_28;
  undefined1 *puStack_24;
  undefined4 local_14;
  LPCSTR local_10;
  LPCSTR local_c;
  int local_8;
  
                    /* 0x38d8  57  AOWInterfaceImgLib.TInterfaceIL.CheckIL@23EDC2EF */
  puStack_24 = &stack0xfffffffc;
  local_c = (LPCSTR)0x0;
  local_10 = (LPCSTR)0x0;
  local_14 = 0;
  puStack_28 = &LAB_598039e7;
  uStack_2c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_2c;
  if (*(int *)(param_1 + 0x44) < param_3 + param_2) {
    local_8 = 0;
    UVar5 = 0;
    System__LStrCat3_51F89FF7(&local_c,"Error - ",*(undefined4 *)(param_4 + 8));
    pCVar4 = local_c;
    SysUtils_IntToStr_0F6FDFF6(param_3 + param_2,&local_14);
    System__LStrCatN_51F89FF7(&local_10,4);
    MessageBoxA((HWND)0x0,local_10,pCVar4,UVar5);
  }
  else {
    iVar3 = param_3 + param_2 + -1;
    puStack_24 = &stack0xfffffffc;
    local_8 = param_1;
    if (param_2 <= iVar3) {
      iVar3 = (iVar3 - param_2) + 1;
      puStack_24 = &stack0xfffffffc;
      do {
        cVar2 = ImageLib_TCustomImageLibrary_ImageExist_23EDC2EF(param_1,param_2);
        if (cVar2 == '\0') {
          UVar5 = 0;
          System__LStrCat3_51F89FF7(&local_c,"Error - ",*(undefined4 *)(param_4 + 8));
          pCVar4 = local_c;
          SysUtils_IntToStr_0F6FDFF6(param_2,&local_14);
          System__LStrCatN_51F89FF7(&local_10,4);
          MessageBoxA((HWND)0x0,local_10,pCVar4,UVar5);
          local_8 = 0;
        }
        param_2 = param_2 + 1;
        iVar3 = iVar3 + -1;
      } while (iVar3 != 0);
    }
  }
  puVar1 = puStack_24;
  *in_FS_OFFSET = uStack_2c;
  puStack_24 = &LAB_598039ee;
  puStack_28 = (undefined1 *)0x598039d6;
  System__LStrClr_40929B27(&local_14,uStack_2c,puVar1);
  puStack_28 = (undefined1 *)0x598039de;
  System__LStrClr_40929B27(&local_10);
  puStack_28 = (undefined1 *)0x598039e6;
  System__LStrClr_40929B27(&local_c);
  return;
}

