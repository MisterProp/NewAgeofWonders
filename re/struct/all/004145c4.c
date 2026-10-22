
void FUN_004145c4(int param_1,undefined4 *param_2,undefined4 *param_3,undefined4 *param_4)

{
  undefined4 uVar1;
  char cVar2;
  
  if ((*(byte *)(*(int *)(param_1 + 0x48) + 0x24) & 1) != 0) {
    AoWSetup_TSetupControl_DataReceived_23EDC2EF
              (*(int *)(param_1 + 0x48),*param_2,*param_3,*param_4);
  }
  if ((*(int *)AoWE_AoWHSMap_6B957A58_exref != 0) &&
     ((*(byte *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x3c) & 4) != 0)) {
    uVar1 = *(undefined4 *)(*(int *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x240) + 0x1c);
    cVar2 = System__IsClass_51F89FF7(uVar1,NetworkE__TRemotePCC_F7D47854_exref);
    if (cVar2 != '\0') {
      NetworkE_TRemotePCC_DataReceived_23EDC2EF(uVar1,*param_2,*param_3,*param_4);
    }
  }
  return;
}

