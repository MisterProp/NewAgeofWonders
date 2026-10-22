
void FUN_0040b390(int param_1)

{
  int iVar1;
  char cVar2;
  
  if (*(char *)(param_1 + 0x91) != '\0') {
    iVar1 = *(int *)(*(int *)(*(int *)PTR_DAT_0045a168 + 0x1dc) + 0x1a8);
    if (iVar1 == 0) {
      if (*(char *)(param_1 + 0x90) == '\x01') {
        iVar1 = *(int *)(*(int *)PTR_DAT_0045a168 + 0x1e4);
        *(undefined4 *)(iVar1 + 0x60) = *(undefined4 *)(param_1 + 0x9c);
        *(undefined4 *)(iVar1 + 100) = *(undefined4 *)(param_1 + 0xa0);
        DisplayC_TCustomDisplay_SetBackSurfaceWidth_23EDC2EF
                  (*(undefined4 *)(*(int *)PTR_DAT_0045a168 + 0x1dc),*(undefined4 *)(iVar1 + 0x60));
        DisplayC_TCustomDisplay_SetBackSurfaceHeight_23EDC2EF
                  (*(undefined4 *)(*(int *)PTR_DAT_0045a168 + 0x1dc),
                   *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a168 + 0x1e4) + 100));
      }
      else if ((*(char *)(param_1 + 0x90) == '\x02') &&
              (cVar2 = AoWReg_TAoWRegistry_GetAllowResolutionSwitches_23EDC2EF
                                 (*(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x5c)),
              cVar2 != '\0')) {
        iVar1 = *(int *)(*(int *)PTR_DAT_0045a168 + 0x1e4);
        *(undefined4 *)(iVar1 + 0x60) = *(undefined4 *)(param_1 + 0x94);
        *(undefined4 *)(iVar1 + 100) = *(undefined4 *)(param_1 + 0x98);
        DisplayC_TCustomDisplay_SetBackSurfaceWidth_23EDC2EF
                  (*(undefined4 *)(*(int *)PTR_DAT_0045a168 + 0x1dc),*(undefined4 *)(iVar1 + 0x60));
        DisplayC_TCustomDisplay_SetBackSurfaceHeight_23EDC2EF
                  (*(undefined4 *)(*(int *)PTR_DAT_0045a168 + 0x1dc),
                   *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a168 + 0x1e4) + 100));
      }
    }
    else {
      cVar2 = *(char *)(param_1 + 0x90);
      if (cVar2 == '\0') {
        FUN_0040b298(param_1,0,0);
      }
      else if (cVar2 == '\x01') {
        if (iVar1 == *(int *)(*(int *)PTR_DAT_0045a168 + 0x1e0)) {
          FUN_0040b298(param_1,0,0);
        }
        else {
          FUN_0040b298(param_1,*(undefined4 *)(param_1 + 0x9c),*(undefined4 *)(param_1 + 0xa0));
        }
      }
      else if (cVar2 == '\x02') {
        if (iVar1 == *(int *)(*(int *)PTR_DAT_0045a168 + 0x1e0)) {
          FUN_0040b298(param_1,0,0);
        }
        else {
          cVar2 = AoWReg_TAoWRegistry_GetAllowResolutionSwitches_23EDC2EF
                            (*(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x5c));
          if ((cVar2 == '\0') ||
             (*(int *)(*(int *)(*(int *)PTR_DAT_0045a168 + 0x1dc) + 0x1a8) ==
              *(int *)(*(int *)PTR_DAT_0045a168 + 0x1e0))) {
            FUN_0040b298(param_1,*(undefined4 *)(param_1 + 0x9c),*(undefined4 *)(param_1 + 0xa0));
          }
          else {
            FUN_0040b298(param_1,*(undefined4 *)(param_1 + 0x94),*(undefined4 *)(param_1 + 0x98));
          }
        }
      }
    }
    *(undefined1 *)(param_1 + 0x91) = 0;
  }
  return;
}

