
void FUN_00412ac8(int param_1,int param_2)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = *(int *)(param_1 + 0x33c);
  uVar2 = AoWSetup_TSetupSettings_IndexOfRace_23EDC2EF
                    (*(undefined4 *)(iVar1 + 0x28),*(undefined1 *)(param_2 + 0xc));
  AoWSetup_TSetupControl_hSetPlayerBehavior_23EDC2EF(iVar1,uVar2,*(undefined1 *)(param_2 + 0x138));
  *(undefined1 *)(*(int *)(*(int *)PTR_DAT_0045a28c + 0x44) + 0xa4) = 0;
  return;
}

