
undefined4 AoWE_TEndTurnAIPA_Process_23EDC2EF(int param_1)

{
  int *piVar1;
  int *piVar2;
  
                    /* 0x34a64  3572  AoWE.TEndTurnAIPA.Process@23EDC2EF */
  if (*(char *)(*(int *)(*(int *)(param_1 + 0xc) + 8) + 0x2c) == '\0') {
    piVar1 = (int *)NetworkE_TTokenEvent_Create_23EDC2EF(&AoWE__TEndTurnTE_3647BA05,1,0);
    *(undefined1 *)(piVar1 + 4) = *(undefined1 *)(*(int *)(*(int *)(param_1 + 0xc) + 8) + 0xa6);
    piVar1[5] = *(int *)(AoWE_AoWHSMap_6B957A58 + 0x174);
    *(undefined1 *)(piVar1 + 6) = 0;
    piVar2 = (int *)NetworkE_TTokenManager_GetTokenControl_23EDC2EF
                              (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x23c),
                               (int)*(char *)(*(int *)(*(int *)(param_1 + 0xc) + 8) + 0xa6));
    (**(code **)(*piVar2 + 0x10))(piVar2,piVar1);
    (**(code **)(*piVar1 + 0x2c))();
    return 1;
  }
  return 0;
}

