

int AoWE_TArmy_ConcealedUnitsCount_23EDC2EF(int param_1)



{

  byte bVar1;

  uint uVar2;

  int iVar3;

  

                    /* 0x8cb84  421  AoWE.TArmy.ConcealedUnitsCount@23EDC2EF */

  bVar1 = *(byte *)(param_1 + 0x2a);

  uVar2 = (uint)bVar1;

  iVar3 = 0;

  while (bVar1 != 0) {

    if ((uVar2 & 1) != 0) {

      iVar3 = iVar3 + 1;

    }

    uVar2 = uVar2 >> 1;

    bVar1 = (byte)uVar2;

  }

  return iVar3;

}




