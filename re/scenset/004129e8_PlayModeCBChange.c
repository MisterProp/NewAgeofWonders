// TScenSetup.PlayModeCBChange  (0x004129e8) - decompiled via GhidraMCP


void FUN_004129e8(int param_1)

{
  int *piVar1;
  undefined4 uVar2;
  
  piVar1 = *(int **)(*(int *)(param_1 + 0xd8) + 0x11c);
  uVar2 = (**(code **)(*piVar1 + 0x18))(piVar1,*(undefined4 *)(*(int *)(param_1 + 0xd8) + 0x138));
  AoWSetup_TSetupControl_hSetPlayMode_23EDC2EF(*(undefined4 *)(param_1 + 0x33c),uVar2);
  *(undefined1 *)(*(int *)(*(int *)PTR_DAT_0045a28c + 0x44) + 0xa4) = 0;
  return;
}

