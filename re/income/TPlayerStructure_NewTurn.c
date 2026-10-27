

void AoWE_TPlayerStructure_NewTurn_23EDC2EF(int param_1,undefined4 param_2)



{

                    /* 0x60a70  2212  AoWE.TPlayerStructure.NewTurn@23EDC2EF */

  AoWE_TStructure_NewTurn_23EDC2EF(param_1,param_2);

  if (((char)param_2 == *(char *)(param_1 + 0x30)) && (*(char *)(param_1 + 0x31) != -1)) {

    *(char *)(param_1 + 0x31) = *(char *)(param_1 + 0x31) + '\x01';

  }

  return;

}




