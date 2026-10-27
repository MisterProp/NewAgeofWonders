// /AoWEPACK.dpl : 5578e2f8

char AoWE_TArmy_CanAddUnit_23EDC2EF(int *param_1,int *param_2,undefined4 param_3)

{
  char cVar1;
  char cVar2;
  byte bVar3;
  int iVar4;
  int iVar5;
  char cVar6;
  
                    /* 0x8e2f8  374  AoWE.TArmy.CanAddUnit@23EDC2EF */
  if ((*(byte *)(param_1 + 4) & 1) == 0) {
    return '\x01';
  }
  iVar4 = (**(code **)(*param_1 + 0x54))();
  iVar5 = AoWE_TArmy_MaxSize_23EDC2EF(param_1);
  if ((iVar4 < iVar5) &&
     (cVar1 = (**(code **)(*param_2 + 0x11c))(param_2,param_1,*param_2,param_3), cVar1 != '\0')) {
    cVar1 = '\x01';
  }
  else {
    cVar1 = '\0';
  }
  iVar4 = AoWE_TArmy_IndexOfTransporter_23EDC2EF(param_1,0xff);
  cVar6 = iVar4 != -1;
  if ((((cVar1 != '\0') && (!(bool)cVar6)) && (*(char *)((int)param_1 + 0x13) != -1)) &&
     ((*(char *)((int)param_1 + 0x13) != '\0' && ((*(byte *)((int)param_1 + 0x11) & 8) == 0)))) {
    iVar4 = HSEngine_TMapContainer_GetField_23EDC2EF
                      (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x10),
                       (int)*(char *)((int)param_1 + 0x13),(int)(char)param_1[5],
                       (int)*(char *)((int)param_1 + 0x15));
    cVar1 = AoWE_TAbstractUnit_ValidTerrain_23EDC2EF
                      (param_2,*(undefined1 *)(iVar4 + 0x14),*(undefined1 *)(iVar4 + 0x15));
  }
  if ((cVar1 != '\0') && (cVar2 = (**(code **)(*param_2 + 0x108))(), cVar2 != '\0')) {
    if (cVar6 == '\0') {
      bVar3 = (**(code **)(*param_2 + 0x104))();
      iVar4 = (**(code **)(*param_1 + 0x54))();
      if (iVar4 <= (int)(uint)bVar3) {
        return '\x01';
      }
    }
    cVar1 = '\0';
  }
  return cVar1;
}

