

void AoWE_TTurnPlayerControl_NewDay_23EDC2EF(void)



{

  undefined1 uVar1;

  

                    /* 0x566d0  2522  AoWE.TTurnPlayerControl.NewDay@23EDC2EF */

  if (*(char *)(AoWE_AoWHSMap_6B957A58 + 0x139) != '\0') {

    uVar1 = *(undefined1 *)(AoWE_AoWHSMap_6B957A58 + 0xa5);

    *(undefined1 *)(AoWE_AoWHSMap_6B957A58 + 0xa5) = 0xff;

    AoWE_TPlayerControl_NewDay_23EDC2EF();

    *(undefined1 *)(AoWE_AoWHSMap_6B957A58 + 0xa5) = uVar1;

    return;

  }

  AoWE_TPlayerControl_NewDay_23EDC2EF();

  return;

}




