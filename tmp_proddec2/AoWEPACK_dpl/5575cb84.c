// /AoWEPACK.dpl : 5575cb84

void AoWE_TProductionControl_Update_23EDC2EF(int param_1)

{
                    /* 0x5cb84  2373  AoWE.TProductionControl.Update@23EDC2EF */
  if ((*(int *)(param_1 + 0x10) == 0) && (*(short *)(param_1 + 10) != 0)) {
    (**(code **)(param_1 + 8))(*(undefined4 *)(param_1 + 0xc),param_1);
  }
  return;
}

