
void FUN_0040b298(int param_1,int param_2,int param_3)

{
  int iVar1;
  
  if (param_2 == 0) {
    if ((*(int **)(*(int *)PTR_DAT_0045a168 + 0x1dc))[0x6a] ==
        *(int *)(*(int *)PTR_DAT_0045a168 + 0x1e4)) {
      (**(code **)(**(int **)(*(int *)PTR_DAT_0045a168 + 0x1dc) + 0xb0))();
    }
  }
  else {
    iVar1 = *(int *)(*(int *)PTR_DAT_0045a168 + 0x1e4);
    if (iVar1 == *(int *)(*(int *)(*(int *)PTR_DAT_0045a168 + 0x1dc) + 0x1a8)) {
      iVar1 = *(int *)(*(int *)PTR_DAT_0045a168 + 0x1e4);
      if ((param_2 != *(int *)(iVar1 + 0x60)) || (param_3 != *(int *)(iVar1 + 100))) {
        DisplayC_TDDrawFullScrDC_SwitchResolution_23EDC2EF(iVar1);
        HSEngine_THSMapViewer_ResetStaticSurface_23EDC2EF
                  (*(undefined4 *)(*(int *)PTR_DAT_0045a5bc + 0x48));
        if (*(char *)(*(int *)(param_1 + 0x44) + 0x75) != '\0') {
          System_TObject_Free_23EDC2EF(*(undefined4 *)(*(int *)(param_1 + 0x44) + 0x18c));
          *(undefined4 *)(*(int *)(param_1 + 0x44) + 0x18c) = 0;
        }
      }
    }
    else {
      *(int *)(iVar1 + 0x60) = param_2;
      *(int *)(iVar1 + 100) = param_3;
      (**(code **)(**(int **)(*(int *)PTR_DAT_0045a168 + 0x1dc) + 0xb0))();
    }
  }
  FUN_0040b08c(param_1);
  FUN_0045431c(*(undefined4 *)PTR_DAT_0045a420);
  return;
}

