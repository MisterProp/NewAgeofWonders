// /AoWEPACK.dpl : AoWE.TAbstractUnit.ShowFace@23EDC2EF

void AoWE_TAbstractUnit_ShowFace_23EDC2EF(int *param_1,undefined4 param_2,undefined4 param_3)

{
  char cVar1;
  int *piVar2;
  int iVar3;
  
                    /* 0x81a44  979  AoWE.TAbstractUnit.ShowFace@23EDC2EF */
  piVar2 = (int *)(**(code **)(*param_1 + 0xf4))();
  if (piVar2 != (int *)0x0) {
    (**(code **)(*piVar2 + 0x88))(piVar2,param_2,param_3);
  }
  cVar1 = (**(code **)(*param_1 + 0x148))(param_1,0x81);
  if ((cVar1 != '\0') && (AoWE_AoWHSMap_6B957A58 != 0)) {
    iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                      (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x48),0x2d);
    if (iVar3 != 0) {
      piVar2 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x48),0x2d);
      (**(code **)(*piVar2 + 0x88))(piVar2,param_2,param_3);
    }
  }
  return;
}

