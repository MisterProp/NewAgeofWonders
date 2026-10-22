
bool ILTer_TILTerrainMO_CanChangeTerrain_23EDC2EF(int *param_1,int param_2)

{
  char cVar1;
  
                    /* 0x18840  955  ILTer.TILTerrainMO.CanChangeTerrain@23EDC2EF */
  cVar1 = HSEngine_TTerrainList_GetTerrain_23EDC2EF
                    (*(undefined4 *)(param_1[2] + 0x28),(int)*(char *)(param_2 + 8));
  if (-1 < cVar1) {
    cVar1 = (**(code **)(*param_1 + 0x124))
                      (param_1,*(undefined4 *)(param_1[1] + 4),(char)param_1[4],(char)param_1[6],
                       *(undefined1 *)((int)param_1 + 0x11));
    return cVar1 == *(char *)(*(int *)(param_2 + 4) + 0x14);
  }
  return true;
}

