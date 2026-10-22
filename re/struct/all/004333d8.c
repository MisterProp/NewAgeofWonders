
void FUN_004333d8(int param_1)

{
  undefined1 uVar1;
  
  AOWWinManager_TAOWWinManager_SetOverPri_23EDC2EF
            (*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0x44),0x31);
  if ((*(char *)(*(int *)(param_1 + 0xa0) + 0xa4) == '\0') ||
     ((*(byte *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x40) & 1) == 0)) {
    uVar1 = 0;
  }
  else {
    uVar1 = 1;
  }
  *(undefined1 *)(*(int *)(param_1 + 0xa0) + 0xa4) = uVar1;
  return;
}

