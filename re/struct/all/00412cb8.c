
void FUN_00412cb8(int param_1)

{
  int *piVar1;
  undefined4 uVar2;
  
  piVar1 = *(int **)(*(int *)(param_1 + 0x328) + 0x11c);
  uVar2 = (**(code **)(*piVar1 + 0x18))(piVar1,*(undefined4 *)(*(int *)(param_1 + 0x328) + 0x138));
  AoWSetup_TSetupControl_hSetTacticalCombatAvailability_23EDC2EF
            (*(undefined4 *)(param_1 + 0x33c),uVar2);
  return;
}

