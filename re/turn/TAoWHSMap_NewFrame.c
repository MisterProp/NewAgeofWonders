
void AoWE_TAoWHSMap_NewFrame_23EDC2EF(int param_1)

{
                    /* 0x77ce8  1404  AoWE.TAoWHSMap.NewFrame@23EDC2EF */
  AoWE_TAbstractAoWHSMap_NewFrame_23EDC2EF(param_1);
  if ((*(byte *)(param_1 + 0x3c) & 4) != 0) {
    (**(code **)(**(int **)(param_1 + 0x244) + 0x4c))();
    (**(code **)(**(int **)(param_1 + 0x240) + 0x50))();
    if ((*(int *)(param_1 + 0x11c) == 0) && (*(char *)(param_1 + 0x118) != '\x02')) {
      AoWE_TAIExecuter_Process_23EDC2EF(*(undefined4 *)(param_1 + 0x128));
    }
    (**(code **)(**(int **)(param_1 + 0x23c) + 0x10))();
    AoWE_TAoWHSMap_UpdateCursor_23EDC2EF(param_1);
    AoWE_TAoWHSMap_UpdateExecuteEventLogDelay_23EDC2EF(param_1);
    AoWE_TAoWHSMap_UpdateTurnTimeBeep_23EDC2EF(param_1);
    AoWE_TAoWHSMap_UpdateEffects_23EDC2EF(param_1);
    if (*(int **)(param_1 + 0x11c) != (int *)0x0) {
      (**(code **)(**(int **)(param_1 + 0x11c) + 0xe8))();
    }
  }
  return;
}

