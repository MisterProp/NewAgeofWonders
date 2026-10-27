

int AoWE_TPlayerMagicControl_GetNetPower_23EDC2EF(void)



{

  int iVar1;

  uint uVar2;

  

                    /* 0x7cec4  1195  AoWE.TPlayerMagicControl.GetNetPower@23EDC2EF */

  iVar1 = AoWE_TPlayerMagicControl_GetPower_23EDC2EF();

  if ((*(byte *)(*(int *)(AoWE_AoWHSMap_6B957A58 + 0x188) + 0x10) & 8) != 0) {

    uVar2 = iVar1 + 1;

    iVar1 = (int)uVar2 >> 1;

    if (iVar1 < 0) {

      iVar1 = iVar1 + (uint)((uVar2 & 1) != 0);

    }

  }

  return iVar1;

}




