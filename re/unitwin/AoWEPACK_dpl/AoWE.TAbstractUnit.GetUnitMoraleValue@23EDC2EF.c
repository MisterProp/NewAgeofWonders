// /AoWEPACK.dpl : AoWE.TAbstractUnit.GetUnitMoraleValue@23EDC2EF

int AoWE_TAbstractUnit_GetUnitMoraleValue_23EDC2EF
              (int *param_1,undefined4 param_2,undefined1 param_3)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  bool bVar6;
  char cStack_16;
  char cStack_15;
  char acStack_14 [4];
  
                    /* 0x7eed8  1093  AoWE.TAbstractUnit.GetUnitMoraleValue@23EDC2EF */
  if ((((char)param_2 < '\0') || (cVar2 = (**(code **)(*param_1 + 0x114))(), cVar2 == '\x02')) ||
     ((*(byte *)(AoWE_AoWHSMap_6B957A58 + 0x3c) & 2) != 0)) {
    return 0x32;
  }
  cVar2 = (**(code **)(*param_1 + 0xa4))();
  iVar3 = AoWE_TAbstractUnit_GetRelationValue_23EDC2EF(param_1,param_2,param_3);
  if ((cVar2 != -1) &&
     (AoWE_TAbstractUnit_GetLocation_23EDC2EF(param_1,&cStack_16,&cStack_15,acStack_14),
     cStack_16 != -1)) {
    iVar4 = HSEngine_TMapContainer_GetField_23EDC2EF
                      (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x10),(int)cStack_16,(int)cStack_15,
                       (int)acStack_14[0]);
    bVar1 = *(byte *)(iVar4 + 0x14);
    uVar5 = (uint)bVar1;
    bVar6 = bVar1 < 0xf;
    if (bVar1 < 0x10) {
      bVar6 = ((byte)(&DAT_558e8396)[((int)(uVar5 & 0x7f) >> 3) + cVar2 * 4] >> (uVar5 & 7) & 1) !=
              0;
    }
    if (bVar6) {
      iVar3 = iVar3 + -10;
    }
    bVar6 = bVar1 < 0xf;
    if (bVar1 < 0x10) {
      bVar6 = ((byte)(&AoWE_RaceTerrain_7F5920D0)[((int)(uVar5 & 0x7f) >> 3) + cVar2 * 4] >>
               (uVar5 & 7) & 1) != 0;
    }
    if (bVar6) {
      iVar3 = iVar3 + 10;
    }
  }
  cVar2 = (**(code **)(*param_1 + 0x148))(param_1,0x35);
  if (cVar2 != '\0') {
    iVar3 = iVar3 + -0x28;
  }
  cVar2 = (**(code **)(*param_1 + 0x148))(param_1,0x6c);
  if (cVar2 != '\0') {
    iVar3 = iVar3 + -0x50;
  }
  if (iVar3 < 0) {
    iVar3 = 0;
  }
  if (iVar3 < 0x65) {
    return iVar3;
  }
  return 100;
}

