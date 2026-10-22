
undefined4 FUN_5574752c(char param_1)

{
  int *piVar1;
  int *piVar2;
  
  piVar1 = (int *)NetworkE_TTokenEvent_Create_23EDC2EF(&AoWE__TEndTurnTE_3647BA05,1,0);
  *(char *)(piVar1 + 4) = param_1;
  piVar1[5] = *(int *)(AoWE_AoWHSMap_6B957A58 + 0x174);
  *(undefined1 *)(piVar1 + 6) = 0;
  piVar2 = (int *)NetworkE_TTokenManager_GetTokenControl_23EDC2EF
                            (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x23c),(int)param_1);
  (**(code **)(*piVar2 + 0x10))(piVar2,piVar1);
  (**(code **)(*piVar1 + 0x2c))();
  return 1;
}

