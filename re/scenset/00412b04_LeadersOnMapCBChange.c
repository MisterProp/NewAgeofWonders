// TScenSetup.LeadersOnMapCBChange  (0x00412b04) - decompiled via GhidraMCP


void FUN_00412b04(int param_1,int param_2)

{
  AoWSetup_TSetupControl_hSetLeadersOnMap_23EDC2EF
            (*(undefined4 *)(param_1 + 0x33c),
             CONCAT31((int3)((uint)*(int *)(param_1 + 0x310) >> 8),
                      *(undefined1 *)(*(int *)(param_1 + 0x310) + 0xb0)));
  if (*(char *)(param_2 + 0xb0) != '\0') {
    FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,1);
    return;
  }
  FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,2);
  return;
}

