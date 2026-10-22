
void FUN_0042a1e8(int param_1)

{
  int *piVar1;
  undefined1 *puVar2;
  int iVar3;
  LPCSTR pCVar4;
  LPCSTR lpExistingFileName;
  undefined4 *in_FS_OFFSET;
  DWORD dwDesiredAccess;
  DWORD dwShareMode;
  LPSECURITY_ATTRIBUTES lpSecurityAttributes;
  DWORD dwCreationDisposition;
  DWORD dwFlagsAndAttributes;
  HANDLE pvVar5;
  BOOL BVar6;
  undefined4 uStack_17c;
  undefined1 *puStack_178;
  undefined1 *puStack_174;
  int local_168;
  undefined1 local_164 [344];
  int local_c;
  undefined4 local_8;
  
  local_168 = 0;
  local_8 = 0;
  local_c = 0;
  puStack_174 = (undefined1 *)0x42a214;
  System__InitializeRecord_51F89FF7(local_164,SysUtils_TSearchRec_08A7EF01_exref);
  puStack_178 = &LAB_0042a3cf;
  uStack_17c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_17c;
  puStack_174 = &stack0xfffffffc;
  AoWReg_TAoWRegistry_GetAttachmentDirectory_23EDC2EF
            (*(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x5c),&local_168);
  if (local_168 != 0) {
    AoWReg_TAoWRegistry_GetAttachmentDirectory_23EDC2EF
              (*(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x5c),&local_168);
    System__LStrCat_51F89FF7(&local_168,"\\*.asg");
    iVar3 = SysUtils_FindFirst_1E70405E(local_168,0x10,local_164);
    while (iVar3 == 0) {
      AoWReg_TAoWRegistry_GetAttachmentDirectory_23EDC2EF
                (*(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x5c),&local_168);
      System__LStrCatN_51F89FF7(&local_8,3);
      pvVar5 = (HANDLE)0x0;
      dwFlagsAndAttributes = 0x80;
      dwCreationDisposition = 3;
      lpSecurityAttributes = (LPSECURITY_ATTRIBUTES)0x0;
      dwShareMode = 0;
      dwDesiredAccess = 0xc0000000;
      pCVar4 = (LPCSTR)System__LStrToPChar_51F89FF7(local_8);
      pvVar5 = CreateFileA(pCVar4,dwDesiredAccess,dwShareMode,lpSecurityAttributes,
                           dwCreationDisposition,dwFlagsAndAttributes,pvVar5);
      if (pvVar5 != (HANDLE)0xffffffff) {
        CloseHandle(pvVar5);
        AoWE_TEmailGameControl_ValidateIncommingGame_23EDC2EF
                  (*(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x74),local_8,&local_c);
        if (local_c != 0) {
          BVar6 = 0;
          pCVar4 = (LPCSTR)System__LStrToPChar_51F89FF7(local_c);
          lpExistingFileName = (LPCSTR)System__LStrToPChar_51F89FF7(local_8);
          BVar6 = CopyFileA(lpExistingFileName,pCVar4,BVar6);
          if (BVar6 != 0) {
            SysUtils_ExtractFileName_435799AA(local_c,&local_168);
            piVar1 = *(int **)(*(int *)(param_1 + 0x68) + 0x114);
            iVar3 = (**(code **)(*piVar1 + 0x50))(piVar1,local_168);
            if (iVar3 == -1) {
              SysUtils_ExtractFileName_435799AA(local_c,&local_168);
              piVar1 = *(int **)(*(int *)(param_1 + 0x68) + 0x114);
              (**(code **)(*piVar1 + 0x34))(piVar1,local_168);
            }
            SysUtils_DeleteFile_3C9BDBFB(local_8);
          }
        }
      }
      iVar3 = SysUtils_FindNext_51BC3B50(local_164);
    }
    SysUtils_FindClose_1671C198(local_164);
  }
  puVar2 = puStack_174;
  *in_FS_OFFSET = uStack_17c;
  puStack_174 = &LAB_0042a3d6;
  puStack_178 = (undefined1 *)0x42a3b0;
  System__LStrClr_40929B27(&local_168,uStack_17c,puVar2);
  puStack_178 = (undefined1 *)0x42a3c1;
  System__FinalizeRecord_51F89FF7(local_164,SysUtils_TSearchRec_08A7EF01_exref);
  puStack_178 = (undefined1 *)0x42a3ce;
  System__LStrArrayClr_51F89FF7(&local_c,2);
  return;
}

