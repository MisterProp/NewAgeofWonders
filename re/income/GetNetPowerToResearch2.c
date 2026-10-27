

void AoWE_TPlayerMagicControl_GetNetPowerToResearch_23EDC2EF

               (int param_1,undefined4 param_2,undefined4 param_3)



{

  longlong lVar1;

  int iVar2;

  

                    /* 0x7cf80  1193  AoWE.TPlayerMagicControl.GetNetPowerToResearch@23EDC2EF */

  iVar2 = AoWE_TPlayerMagicControl_GetNetPower_23EDC2EF(param_1,param_2,param_3,param_3);

  lVar1 = (longlong)iVar2 * (longlong)*(int *)(param_1 + 0x2c);

  System__ROUND_51F89FF7((int)lVar1,(int)((ulonglong)lVar1 >> 0x20));

  return;

}




