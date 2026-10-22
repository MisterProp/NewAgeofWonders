
void FUN_0041f1f4(int param_1)

{
  int *piVar1;
  undefined1 *puVar2;
  DWORD DVar3;
  UINT UVar4;
  int iVar5;
  uint uVar6;
  undefined4 *puVar7;
  CHAR *pCVar8;
  undefined4 *in_FS_OFFSET;
  bool bVar9;
  undefined1 uVar10;
  byte bVar11;
  undefined *puVar12;
  LPCSTR pCVar13;
  undefined4 uVar14;
  undefined4 uStack_b0;
  undefined1 *puStack_ac;
  undefined1 *puStack_a8;
  undefined1 local_98 [32];
  undefined1 local_78;
  undefined1 local_77;
  undefined1 local_74 [28];
  undefined1 local_58 [28];
  undefined4 local_3c;
  undefined4 local_38;
  CHAR local_32 [26];
  int local_18;
  undefined1 local_11;
  DWORD local_10;
  DWORD local_c;
  LPCSTR local_8;
  
  bVar11 = 0;
  puStack_a8 = &stack0xfffffffc;
  local_38 = 0;
  local_3c = 0;
  local_8 = (LPCSTR)0x0;
  puStack_ac = &LAB_0041f3dd;
  uStack_b0 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_b0;
  (**(code **)(*DAT_0045b108 + 0x40))();
  DVar3 = GetLogicalDrives();
  local_18 = 1;
  uVar6 = 0;
  do {
    bVar9 = uVar6 < 0x1f;
    if (uVar6 < 0x20) {
      bVar9 = (DVar3 >> (uVar6 & 0x1f) & 1) != 0;
    }
    if (bVar9) {
      puVar7 = &DAT_0041f3ec;
      pCVar8 = local_32;
      for (iVar5 = 6; iVar5 != 0; iVar5 = iVar5 + -1) {
        *(undefined4 *)pCVar8 = *puVar7;
        puVar7 = puVar7 + (uint)bVar11 * -2 + 1;
        pCVar8 = pCVar8 + ((uint)bVar11 * -2 + 1) * 4;
      }
      *(undefined2 *)pCVar8 = *(undefined2 *)puVar7;
      System__LStrFromChar_001FB870(&local_38,CONCAT31((int3)(uVar6 >> 8),(char)uVar6 + 'A'));
      System__LStrCat3_51F89FF7(&local_8,local_38,&DAT_0041f410);
      uVar10 = uVar6 == 1;
      if ((int)uVar6 < 2) {
        local_11 = 0;
      }
      else {
        local_11 = 0x20;
        UVar4 = GetDriveTypeA(local_8);
        uVar10 = UVar4 == 4;
        if ((bool)uVar10) {
          local_11 = 0;
        }
        else {
          GetVolumeInformationA(local_8,local_32,0x19,(LPDWORD)0x0,&local_c,&local_10,(LPSTR)0x0,0);
        }
      }
      System___CLenToPasStr_51F89FF7(local_58,local_32,0x1a);
      System__PStrCpy_51F89FF7(local_74,local_58);
      local_77 = local_11;
      local_78 = 1;
      System__PStrNCat_51F89FF7(local_74,&local_78,0x1b);
      System__PStrCpy_51F89FF7(local_98,local_74);
      System__PStrNCat_51F89FF7(local_98,&DAT_0041f414,0x1c);
      System__LStrFromString_EF219F52(&local_3c,local_98);
      puVar12 = &DAT_0041f420;
      pCVar13 = local_8;
      uVar14 = local_3c;
      System__LStrCatN_51F89FF7(&local_38,3);
      (**(code **)(*DAT_0045b108 + 0x34))
                (DAT_0045b108,local_38,*DAT_0045b108,puVar12,pCVar13,uVar14);
      SysUtils_GetCurrentDir_6835DD73(&local_3c);
      SysUtils_ExtractFileDrive_435799AA(local_3c,&local_38);
      System__LStrCat_51F89FF7(&local_38,&DAT_0041f42c);
      System__LStrCmp_51F89FF7(local_8,local_38);
      if ((bool)uVar10) {
        local_18 = (**(code **)(*DAT_0045b108 + 0x14))();
        local_18 = local_18 + -1;
      }
    }
    uVar6 = uVar6 + 1;
  } while (uVar6 != 0x1a);
  (**(code **)(**(int **)(*(int *)(param_1 + 100) + 0x11c) + 0x40))();
  piVar1 = *(int **)(*(int *)(param_1 + 100) + 0x11c);
  (**(code **)(*piVar1 + 0x3c))(piVar1,DAT_0045b108);
  (**(code **)(**(int **)(param_1 + 100) + 0xb8))(*(int **)(param_1 + 100),local_18);
  FUN_0041f430(param_1);
  puVar2 = puStack_a8;
  *in_FS_OFFSET = uStack_b0;
  puStack_a8 = &LAB_0041f3e4;
  puStack_ac = (undefined1 *)0x41f3d4;
  System__LStrArrayClr_51F89FF7(&local_3c,2,puVar2);
  puStack_ac = (undefined1 *)0x41f3dc;
  System__LStrClr_40929B27(&local_8);
  return;
}

