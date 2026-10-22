
void FUN_00438b2c(int param_1,int param_2,int *param_3,int param_4)

{
  int iVar1;
  int *piVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  undefined4 *puVar6;
  int *piVar7;
  undefined4 *puVar8;
  byte bVar9;
  undefined1 *puVar10;
  undefined1 local_2c;
  undefined1 local_2b;
  undefined4 local_20 [4];
  int local_10;
  int local_c;
  int local_8;
  
  bVar9 = 0;
  local_8 = param_2;
  iVar1 = (**(code **)(**(int **)(param_1 + 0xa4) + 0x54))();
  if (local_8 < iVar1) {
    puVar6 = (undefined4 *)(param_4 + 0x10);
    puVar8 = local_20;
    for (iVar1 = 4; iVar1 != 0; iVar1 = iVar1 + -1) {
      *puVar8 = *puVar6;
      puVar6 = puVar6 + (uint)bVar9 * -2 + 1;
      puVar8 = puVar8 + (uint)bVar9 * -2 + 1;
    }
    piVar2 = param_3;
    piVar7 = (int *)(param_4 + 0x10);
    for (iVar1 = 4; iVar1 != 0; iVar1 = iVar1 + -1) {
      *piVar7 = *piVar2;
      piVar2 = piVar2 + (uint)bVar9 * -2 + 1;
      piVar7 = piVar7 + (uint)bVar9 * -2 + 1;
    }
    local_c = param_3[2] + *param_3 >> 1;
    if (local_c < 0) {
      local_c = local_c + (uint)((param_3[2] + *param_3 & 1U) != 0);
    }
    local_10 = param_3[1] + param_3[3] >> 1;
    if (local_10 < 0) {
      local_10 = local_10 + (uint)((param_3[1] + param_3[3] & 1U) != 0);
    }
    piVar2 = (int *)AoWE_TUnitList_GetUnit_23EDC2EF(*(undefined4 *)(param_1 + 0xa4),local_8);
    uVar3 = (**(code **)(*piVar2 + 0xfc))();
    if ((int)(uVar3 & 0xff) < *(int *)(*(int *)(*(int *)PTR_DAT_0045a4a0 + 100) + 0x44)) {
      iVar1 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                        (*(int *)(*(int *)PTR_DAT_0045a4a0 + 100),uVar3 & 0x7f,PTR_DAT_0045a4a0);
      iVar4 = (int)*(uint *)(iVar1 + 0x24) >> 1;
      if (iVar4 < 0) {
        iVar4 = iVar4 + (uint)((*(uint *)(iVar1 + 0x24) & 1) != 0);
      }
      iVar5 = (int)*(uint *)(iVar1 + 0x20) >> 1;
      if (iVar5 < 0) {
        iVar5 = iVar5 + (uint)((*(uint *)(iVar1 + 0x20) & 1) != 0);
      }
      ImageLib_TLibraryImage_ShowClipped_23EDC2EF(iVar1,local_c - iVar5,local_10 - iVar4,param_4);
    }
    System__FillChar_51F89FF7(&local_2c,0xc,0);
    local_2c = 3;
    local_2b = 3;
    puVar10 = &local_2c;
    iVar1 = param_4;
    piVar2 = (int *)AoWE_TUnitList_GetUnit_23EDC2EF(*(undefined4 *)(param_1 + 0xa4),local_8);
    (**(code **)(*piVar2 + 0x1a4))(piVar2,local_c + -0x21,local_10 + -0x23,puVar10,iVar1);
    if ((*(byte *)(param_1 + 0xa1) >> ((byte)local_8 & 0x1f) & 1) == 0) {
      iVar1 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                        (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 100),7);
      iVar4 = (int)*(uint *)(iVar1 + 0x24) >> 1;
      if (iVar4 < 0) {
        iVar4 = iVar4 + (uint)((*(uint *)(iVar1 + 0x24) & 1) != 0);
      }
      iVar5 = (int)*(uint *)(iVar1 + 0x20) >> 1;
      if (iVar5 < 0) {
        iVar5 = iVar5 + (uint)((*(uint *)(iVar1 + 0x20) & 1) != 0);
      }
      ImageLib_TLibraryImage_ShowClipped_23EDC2EF(iVar1,local_c - iVar5,local_10 - iVar4,param_4);
    }
    puVar6 = local_20;
    puVar8 = (undefined4 *)(param_4 + 0x10);
    for (iVar1 = 4; iVar1 != 0; iVar1 = iVar1 + -1) {
      *puVar8 = *puVar6;
      puVar6 = puVar6 + (uint)bVar9 * -2 + 1;
      puVar8 = puVar8 + (uint)bVar9 * -2 + 1;
    }
  }
  return;
}

