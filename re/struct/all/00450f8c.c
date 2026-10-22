
void FUN_00450f8c(int param_1)

{
  int *piVar1;
  undefined4 uVar2;
  int iVar3;
  
  if (*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5) != -1) {
    iVar3 = *(int *)(*(int *)(DAT_0045b298 + 0x48) + 0x28);
    if (iVar3 < 0) {
      iVar3 = iVar3 + 0x1f;
    }
    Scanner_TScanner_SetTWidth_23EDC2EF(*(undefined4 *)(*(int *)PTR_DAT_0045a1c0 + 0x5c),iVar3 >> 5)
    ;
    iVar3 = *(int *)(*(int *)(DAT_0045b298 + 0x48) + 0x2c);
    if (iVar3 < 0) {
      iVar3 = iVar3 + 0x1f;
    }
    Scanner_TScanner_SetTHeight_23EDC2EF
              (*(undefined4 *)(*(int *)PTR_DAT_0045a1c0 + 0x5c),iVar3 >> 5);
    iVar3 = *(int *)(*(int *)(param_1 + 0x48) + 0x30);
    if (iVar3 < 0) {
      iVar3 = iVar3 + 0x1f;
    }
    Scanner_TScanner_SetTLeft_23EDC2EF
              (*(undefined4 *)(*(int *)PTR_DAT_0045a1c0 + 0x5c),(iVar3 >> 5) + -6);
    iVar3 = *(int *)(*(int *)(param_1 + 0x48) + 0x34);
    if (iVar3 < 0) {
      iVar3 = iVar3 + 0x1f;
    }
    Scanner_TScanner_SetTTop_23EDC2EF
              (*(undefined4 *)(*(int *)PTR_DAT_0045a1c0 + 0x5c),(iVar3 >> 5) + -6);
    iVar3 = *(int *)(*(int *)(DAT_0045b298 + 0x48) + 0x28);
    if (iVar3 < 0) {
      iVar3 = iVar3 + 0x1f;
    }
    Scanner_TScanner_SetTWidth_23EDC2EF(*(undefined4 *)(*(int *)PTR_DAT_0045a098 + 0x4c),iVar3 >> 5)
    ;
    iVar3 = *(int *)(*(int *)(DAT_0045b298 + 0x48) + 0x2c);
    if (iVar3 < 0) {
      iVar3 = iVar3 + 0x1f;
    }
    Scanner_TScanner_SetTHeight_23EDC2EF
              (*(undefined4 *)(*(int *)PTR_DAT_0045a098 + 0x4c),iVar3 >> 5);
    iVar3 = *(int *)(*(int *)(DAT_0045b298 + 0x48) + 0x30);
    if (iVar3 < 0) {
      iVar3 = iVar3 + 0x1f;
    }
    Scanner_TScanner_SetTLeft_23EDC2EF
              (*(undefined4 *)(*(int *)PTR_DAT_0045a098 + 0x4c),(iVar3 >> 5) + -6);
    iVar3 = *(int *)(*(int *)(DAT_0045b298 + 0x48) + 0x34);
    if (iVar3 < 0) {
      iVar3 = iVar3 + 0x1f;
    }
    Scanner_TScanner_SetTTop_23EDC2EF
              (*(undefined4 *)(*(int *)PTR_DAT_0045a098 + 0x4c),(iVar3 >> 5) + -6);
  }
  if (*(int *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x11c) == 0) {
    iVar3 = HSEngine_TMapContainer_GetMapLevel_23EDC2EF
                      (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x10),
                       *(undefined4 *)(*(int *)(param_1 + 0x48) + 0x38));
    if (iVar3 != *(int *)(*(int *)(*(int *)PTR_DAT_0045a1c0 + 0x5c) + 0x15c)) {
      iVar3 = *(int *)(*(int *)PTR_DAT_0045a1c0 + 0x5c);
      *(undefined4 *)(iVar3 + 0x17c) = 0xffffffff;
      uVar2 = HSEngine_TMapContainer_GetMapLevel_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x10),
                         *(undefined4 *)(*(int *)(param_1 + 0x48) + 0x38));
      Scanner_TScanner_SetLevel_23EDC2EF(iVar3,uVar2);
      AoWTabPanel_TAOWTabPanel_SetIndex_23EDC2EF
                (*(undefined4 *)(*(int *)PTR_DAT_0045a1c0 + 0x4c),
                 *(undefined4 *)(*(int *)(param_1 + 0x48) + 0x38));
    }
  }
  if (*(int *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x11c) != 0) {
    piVar1 = *(int **)(*(int *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x11c) + 0xc);
    (**(code **)(*piVar1 + 0x20))(piVar1,piVar1[3] & 0xffffff00,piVar1[2],piVar1[3]);
    return;
  }
  piVar1 = *(int **)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xc);
  (**(code **)(*piVar1 + 0x20))(piVar1,piVar1[3] & 0xffffff00,piVar1[2],piVar1[3]);
  return;
}

