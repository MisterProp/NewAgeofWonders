
byte Mountain_TMountainMO_ValidMountainOverlap_23EDC2EF
               (undefined4 param_1,int *param_2,char param_3)

{
  int *piVar1;
  char cVar2;
  undefined4 uVar3;
  int iVar4;
  byte bVar5;
  int iVar6;
  
                    /* 0xa2de0  6620  Mountain.TMountainMO.ValidMountainOverlap@23EDC2EF */
  bVar5 = 1;
  iVar6 = 0;
  while( true ) {
    iVar4 = (**(code **)(*param_2 + 0x54))();
    if ((iVar6 != iVar4 & bVar5) == 0) break;
    piVar1 = *(int **)(param_2[2] + iVar6 * 4);
    iVar4 = (**(code **)(*piVar1 + 0x24))();
    if (iVar4 == 0x20168) {
      uVar3 = HSEngine_TMapField_GetHNofHS_23EDC2EF(param_2,piVar1);
      cVar2 = HSEngine_TTerrainList_GetOverlay_23EDC2EF(*(undefined4 *)(piVar1[2] + 0x28),uVar3);
      if (((cVar2 == '\0') && (param_3 == '\0')) || ((cVar2 == '\x01' && (param_3 == '\x01')))) {
        bVar5 = 0;
      }
      else {
        bVar5 = 1;
      }
    }
    iVar6 = iVar6 + 1;
  }
  return bVar5;
}

