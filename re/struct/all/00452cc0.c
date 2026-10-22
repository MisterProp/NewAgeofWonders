
void FUN_00452cc0(int param_1,int param_2,int param_3)

{
  char cVar1;
  int *piVar2;
  int iVar3;
  
  cVar1 = FUN_00455eac(*(undefined4 *)PTR_DAT_0045a420);
  if (((cVar1 == '\0') && (param_2 != -1)) &&
     (*(char *)(*(int *)Forms_Application_E55EEEF8_exref + 0x7d) != '\0')) {
    if (param_2 < 2) {
      HSEngine_THSMapViewer_SetSceneX_23EDC2EF
                (*(int *)(*(int *)PTR_DAT_0045a5bc + 0x48),
                 *(int *)(*(int *)(*(int *)PTR_DAT_0045a5bc + 0x48) + 0x30) + -0x10,PTR_DAT_0045a5bc
                );
      iVar3 = *(int *)(*(int *)(*(int *)PTR_DAT_0045a5bc + 0x48) + 0x30);
      if (iVar3 < 0) {
        iVar3 = iVar3 + 0x1f;
      }
      Scanner_TScanner_SetTLeft_23EDC2EF
                (*(undefined4 *)(*(int *)PTR_DAT_0045a1c0 + 0x5c),(iVar3 >> 5) + -6);
    }
    piVar2 = *(int **)(*(int *)(param_1 + 0x1dc) + 0x1a8);
    piVar2 = (int *)(**(code **)(*piVar2 + 0x40))(piVar2,0);
    iVar3 = (**(code **)(*piVar2 + 8))();
    if (iVar3 + -1 <= param_2) {
      HSEngine_THSMapViewer_SetSceneX_23EDC2EF
                (*(int *)(*(int *)PTR_DAT_0045a5bc + 0x48),
                 *(int *)(*(int *)(*(int *)PTR_DAT_0045a5bc + 0x48) + 0x30) + 0x10,PTR_DAT_0045a5bc)
      ;
      iVar3 = *(int *)(*(int *)(*(int *)PTR_DAT_0045a5bc + 0x48) + 0x30);
      if (iVar3 < 0) {
        iVar3 = iVar3 + 0x1f;
      }
      Scanner_TScanner_SetTLeft_23EDC2EF
                (*(undefined4 *)(*(int *)PTR_DAT_0045a1c0 + 0x5c),(iVar3 >> 5) + -6);
    }
    if (param_3 < 2) {
      HSEngine_THSMapViewer_SetSceneY_23EDC2EF
                (*(int *)(*(int *)PTR_DAT_0045a5bc + 0x48),
                 *(int *)(*(int *)(*(int *)PTR_DAT_0045a5bc + 0x48) + 0x34) + -0x10,PTR_DAT_0045a5bc
                );
      iVar3 = *(int *)(*(int *)(*(int *)PTR_DAT_0045a5bc + 0x48) + 0x34);
      if (iVar3 < 0) {
        iVar3 = iVar3 + 0x1f;
      }
      Scanner_TScanner_SetTTop_23EDC2EF
                (*(undefined4 *)(*(int *)PTR_DAT_0045a1c0 + 0x5c),(iVar3 >> 5) + -6);
    }
    piVar2 = *(int **)(*(int *)(param_1 + 0x1dc) + 0x1a8);
    piVar2 = (int *)(**(code **)(*piVar2 + 0x40))(piVar2,0);
    iVar3 = (**(code **)(*piVar2 + 0xc))();
    if (iVar3 + -1 <= param_3) {
      HSEngine_THSMapViewer_SetSceneY_23EDC2EF
                (*(int *)(*(int *)PTR_DAT_0045a5bc + 0x48),
                 *(int *)(*(int *)(*(int *)PTR_DAT_0045a5bc + 0x48) + 0x34) + 0x10,PTR_DAT_0045a5bc)
      ;
      iVar3 = *(int *)(*(int *)(*(int *)PTR_DAT_0045a5bc + 0x48) + 0x34);
      if (iVar3 < 0) {
        iVar3 = iVar3 + 0x1f;
      }
      Scanner_TScanner_SetTTop_23EDC2EF
                (*(undefined4 *)(*(int *)PTR_DAT_0045a1c0 + 0x5c),(iVar3 >> 5) + -6);
    }
  }
  return;
}

