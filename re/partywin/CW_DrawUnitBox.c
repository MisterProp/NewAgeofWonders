
void FUN_00405d98(undefined4 param_1,int param_2,int *param_3,int param_4)

{
  int iVar1;
  int iVar2;
  undefined1 uVar3;
  int *piVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  undefined4 uVar8;
  int *piVar9;
  byte bVar10;
  
  bVar10 = 0;
  iVar5 = *(int *)(*(int *)PTR_DAT_0045a420 + 0x84 + param_2 * 4);
  if (iVar5 != -1) {
    piVar4 = (int *)AoWE_TUnitList_GetUnit_23EDC2EF
                              (*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0xa4),iVar5);
    iVar5 = (**(code **)(*piVar4 + 0x100))();
    if (iVar5 != 0) {
      piVar4 = param_3;
      piVar9 = (int *)(param_4 + 0x10);
      for (iVar5 = 4; iVar5 != 0; iVar5 = iVar5 + -1) {
        *piVar9 = *piVar4;
        piVar4 = piVar4 + (uint)bVar10 * -2 + 1;
        piVar9 = piVar9 + (uint)bVar10 * -2 + 1;
      }
      piVar4 = (int *)AoWE_TUnitList_GetUnit_23EDC2EF
                                (*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0xa4),param_2);
      uVar6 = (**(code **)(*piVar4 + 0xfc))();
      if ((int)(uVar6 & 0x7f) < *(int *)(*(int *)(*(int *)PTR_DAT_0045a4a0 + 100) + 0x44)) {
        piVar4 = (int *)AoWE_TUnitList_GetUnit_23EDC2EF
                                  (*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0xa4),
                                   *(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0x84 + param_2 * 4));
        uVar3 = (**(code **)(*piVar4 + 0xfc))();
        iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                          (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 100),uVar3);
        uVar6 = (param_3[3] - param_3[1]) - *(int *)(iVar5 + 0x24);
        iVar5 = (int)uVar6 >> 1;
        if (iVar5 < 0) {
          iVar5 = iVar5 + (uint)((uVar6 & 1) != 0);
        }
        iVar1 = param_3[1];
        piVar4 = (int *)AoWE_TUnitList_GetUnit_23EDC2EF
                                  (*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0xa4),
                                   *(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0x84 + param_2 * 4));
        uVar3 = (**(code **)(*piVar4 + 0xfc))();
        iVar7 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                          (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 100),uVar3);
        iVar2 = *param_3;
        uVar6 = (param_3[2] - iVar2) - *(int *)(iVar7 + 0x20);
        iVar7 = (int)uVar6 >> 1;
        if (iVar7 < 0) {
          iVar7 = iVar7 + (uint)((uVar6 & 1) != 0);
        }
        piVar4 = (int *)AoWE_TUnitList_GetUnit_23EDC2EF
                                  (*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0xa4),
                                   *(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0x84 + param_2 * 4));
        uVar3 = (**(code **)(*piVar4 + 0xfc))();
        uVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                          (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 100),uVar3);
        ImageLib_TLibraryImage_ShowClipped_23EDC2EF(uVar8,iVar7 + iVar2,iVar5 + iVar1);
      }
      piVar4 = (int *)AoWE_TUnitList_GetUnit_23EDC2EF
                                (*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0xa4),
                                 *(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0x84 + param_2 * 4));
      iVar5 = (**(code **)(*piVar4 + 0x100))();
      uVar6 = (param_3[3] - param_3[1]) - *(int *)(iVar5 + 0x24);
      iVar5 = (int)uVar6 >> 1;
      if (iVar5 < 0) {
        iVar5 = iVar5 + (uint)((uVar6 & 1) != 0);
      }
      iVar1 = param_3[1];
      piVar4 = (int *)AoWE_TUnitList_GetUnit_23EDC2EF
                                (*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0xa4),
                                 *(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0x84 + param_2 * 4));
      iVar7 = (**(code **)(*piVar4 + 0x100))();
      iVar2 = *param_3;
      uVar6 = (param_3[2] - iVar2) - *(int *)(iVar7 + 0x20);
      iVar7 = (int)uVar6 >> 1;
      if (iVar7 < 0) {
        iVar7 = iVar7 + (uint)((uVar6 & 1) != 0);
      }
      piVar4 = (int *)AoWE_TUnitList_GetUnit_23EDC2EF
                                (*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0xa4),
                                 *(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0x84 + param_2 * 4));
      (**(code **)(*piVar4 + 0x1a4))(piVar4,iVar7 + iVar2,iVar5 + iVar1);
    }
  }
  return;
}

