
int ILTer_TILTerrainMO_GetValidTerrainType_23EDC2EF
              (int *param_1,undefined4 param_2,uint param_3,undefined1 param_4)

{
  char cVar1;
  char cVar2;
  int iVar3;
  int iVar4;
  
                    /* 0x18780  957  ILTer.TILTerrainMO.GetValidTerrainType@23EDC2EF */
  iVar3 = (**(code **)(*param_1 + 0x124))(param_1,param_2,param_3,0xffffffff,param_4);
  cVar1 = (**(code **)(*param_1 + 0x128))(param_1,(int)(char)iVar3);
  if (cVar1 == '\0') {
    iVar3 = (**(code **)(*param_1 + 0x130))(param_1,param_2,param_3 & 0xff,param_4);
    cVar1 = (**(code **)(*param_1 + 0x84))();
    if ((cVar1 != '\0') || ((char)iVar3 != -2)) {
      cVar1 = (**(code **)(*param_1 + 0x128))(param_1,(int)(char)iVar3);
      if (cVar1 == '\0') {
        iVar3 = 0;
        iVar4 = (**(code **)(**(int **)(*(int *)(*(int *)(param_1[2] + 0x18) + 0x38) + 0x3c) + 0x14)
                )();
        while( true ) {
          cVar1 = (char)iVar3;
          if (iVar4 <= cVar1) break;
          cVar2 = (**(code **)(*param_1 + 0x128))(param_1,(int)cVar1);
          if (cVar2 != '\0') break;
          iVar3 = iVar3 + 1;
        }
        if (cVar1 == iVar4) {
          iVar3 = -1;
        }
      }
    }
  }
  return iVar3;
}

