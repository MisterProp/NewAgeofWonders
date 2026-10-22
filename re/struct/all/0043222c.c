
void FUN_0043222c(int param_1,int param_2,int param_3,int param_4,int param_5)

{
  int iVar1;
  int *piVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  int iVar6;
  undefined4 *puVar7;
  byte bVar8;
  undefined1 *puVar9;
  undefined4 local_38 [4];
  undefined4 local_28 [4];
  undefined1 local_18;
  undefined1 local_17;
  int local_c;
  int local_8;
  
  bVar8 = 0;
  local_c = param_3;
  local_8 = param_2;
  iVar1 = (**(code **)(**(int **)(*(int *)(param_1 + 0xe0) + 0x14) + 0x54))();
  if (local_8 < iVar1) {
    puVar5 = (undefined4 *)(param_4 + 0x10);
    puVar7 = local_28;
    for (iVar1 = 4; iVar1 != 0; iVar1 = iVar1 + -1) {
      *puVar7 = *puVar5;
      puVar5 = puVar5 + (uint)bVar8 * -2 + 1;
      puVar7 = puVar7 + (uint)bVar8 * -2 + 1;
    }
    Classes_Rect_9923E97E(local_c + 5,param_5 + 4,local_c + 0x2e,local_38,param_5 + 0x38);
    puVar5 = local_38;
    puVar7 = (undefined4 *)(param_4 + 0x10);
    for (iVar1 = 4; iVar1 != 0; iVar1 = iVar1 + -1) {
      *puVar7 = *puVar5;
      puVar5 = puVar5 + (uint)bVar8 * -2 + 1;
      puVar7 = puVar7 + (uint)bVar8 * -2 + 1;
    }
    piVar2 = (int *)AoWE_TUnitList_GetUnit_23EDC2EF
                              (*(undefined4 *)(*(int *)(param_1 + 0xe0) + 0x14),local_8);
    uVar3 = (**(code **)(*piVar2 + 0xfc))();
    if ((int)(uVar3 & 0xff) < *(int *)(*(int *)(*(int *)PTR_DAT_0045a4a0 + 100) + 0x44)) {
      iVar1 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                        (*(int *)(*(int *)PTR_DAT_0045a4a0 + 100),uVar3 & 0x7f,PTR_DAT_0045a4a0);
      iVar4 = (int)*(uint *)(iVar1 + 0x24) >> 1;
      if (iVar4 < 0) {
        iVar4 = iVar4 + (uint)((*(uint *)(iVar1 + 0x24) & 1) != 0);
      }
      iVar6 = (int)*(uint *)(iVar1 + 0x20) >> 1;
      if (iVar6 < 0) {
        iVar6 = iVar6 + (uint)((*(uint *)(iVar1 + 0x20) & 1) != 0);
      }
      ImageLib_TLibraryImage_ShowClipped_23EDC2EF
                (iVar1,(local_c + 0x19) - iVar6,(param_5 + 0x1e) - iVar4,param_4);
    }
    System__FillChar_51F89FF7(&local_18,0xc,0);
    local_18 = 3;
    local_17 = 3;
    puVar9 = &local_18;
    iVar1 = param_4;
    piVar2 = (int *)AoWE_TUnitList_GetUnit_23EDC2EF
                              (*(undefined4 *)(*(int *)(param_1 + 0xe0) + 0x14),local_8);
    (**(code **)(*piVar2 + 0x1a4))(piVar2,local_c + -8,param_5 + -4,puVar9,iVar1);
    if ((*(byte *)(*(int *)(param_1 + 0xe0) + 0x1a) >> ((byte)local_8 & 0x1f) & 1) == 0) {
      iVar1 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                        (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 100),7);
      iVar4 = (int)*(uint *)(iVar1 + 0x24) >> 1;
      if (iVar4 < 0) {
        iVar4 = iVar4 + (uint)((*(uint *)(iVar1 + 0x24) & 1) != 0);
      }
      iVar6 = (int)*(uint *)(iVar1 + 0x20) >> 1;
      if (iVar6 < 0) {
        iVar6 = iVar6 + (uint)((*(uint *)(iVar1 + 0x20) & 1) != 0);
      }
      ImageLib_TLibraryImage_ShowClipped_23EDC2EF
                (iVar1,(local_c + 0x19) - iVar6,(param_5 + 0x1e) - iVar4,param_4);
    }
    puVar5 = local_28;
    puVar7 = (undefined4 *)(param_4 + 0x10);
    for (iVar1 = 4; iVar1 != 0; iVar1 = iVar1 + -1) {
      *puVar7 = *puVar5;
      puVar5 = puVar5 + (uint)bVar8 * -2 + 1;
      puVar7 = puVar7 + (uint)bVar8 * -2 + 1;
    }
  }
  return;
}

