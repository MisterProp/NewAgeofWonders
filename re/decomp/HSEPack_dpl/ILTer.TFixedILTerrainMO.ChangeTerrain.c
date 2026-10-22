
void ILTer_TFixedILTerrainMO_ChangeTerrain_23EDC2EF(int *param_1,char *param_2)

{
  char cVar1;
  int iVar2;
  
                    /* 0x18d14  938  ILTer.TFixedILTerrainMO.ChangeTerrain@23EDC2EF */
  cVar1 = HSEngine_TTerrainList_GetTerrain_23EDC2EF
                    (*(undefined4 *)(param_1[2] + 0x28),(int)param_2[8]);
  if (-1 < cVar1) {
    cVar1 = (**(code **)(*param_1 + 0x128))(param_1,(int)*param_2);
    if (cVar1 != '\0') {
      *(char *)(param_1 + 6) = *param_2;
      iVar2 = (**(code **)(*param_1 + 300))(param_1,(int)*param_2);
      param_1[5] = iVar2;
    }
  }
  return;
}

