
void FUN_0042a510(int param_1)

{
  int iVar1;
  
  *(int *)(param_1 + 0xa4) = *(int *)(param_1 + 0xa4) + -1;
  if (*(int *)(param_1 + 0xa4) < 0) {
    iVar1 = AoWReg_TAoWRegistry_GetCheckEmailFrequency_23EDC2EF
                      (*(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x5c));
    *(int *)(param_1 + 0xa4) = iVar1 * 0x708;
    if (iVar1 * 0x708 < 0x1e) {
      *(undefined4 *)(param_1 + 0xa4) = 0x1e;
    }
    *(undefined4 *)(param_1 + 0xa4) = 0x96;
  }
  if (*(int *)(param_1 + 0xa4) == 0x19) {
    FUN_0042a1e8(param_1);
  }
  return;
}

