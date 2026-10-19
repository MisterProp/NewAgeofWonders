
void FUN_004066f4(void)

{
  int iVar1;
  undefined *puVar2;
  
  puVar2 = PTR_DAT_0045a420;
  FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,0);
  iVar1 = *(int *)(*(int *)puVar2 + 0xa4);
  (**(code **)(**(int **)AoWE_AoWHSMap_6B957A58_exref + 0xbc))
            (*(int **)AoWE_AoWHSMap_6B957A58_exref,(int)*(char *)(iVar1 + 0x13),
             (int)*(char *)(iVar1 + 0x14),(int)*(char *)(iVar1 + 0x15));
  iVar1 = *(int *)(*(int *)puVar2 + 0xa4);
  AoWE_TAbstractAoWHSMap_PlayCenterAnimation_23EDC2EF
            (*(undefined4 *)AoWE_AoWHSMap_6B957A58_exref,*(undefined1 *)(iVar1 + 0x13),
             *(undefined1 *)(iVar1 + 0x14),*(undefined1 *)(iVar1 + 0x15));
  return;
}

