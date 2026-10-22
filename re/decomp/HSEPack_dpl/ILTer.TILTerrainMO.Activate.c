
void ILTer_TILTerrainMO_Activate_23EDC2EF(int *param_1)

{
  char cVar1;
  int iVar2;
  
                    /* 0x18c10  946  ILTer.TILTerrainMO.Activate@23EDC2EF */
  HSEngine_TMapObject_Activate_23EDC2EF(param_1);
  cVar1 = (**(code **)(*param_1 + 0x134))
                    (param_1,*(undefined4 *)(param_1[1] + 4),(char)param_1[4],
                     *(undefined1 *)((int)param_1 + 0x11));
  *(char *)(param_1 + 6) = cVar1;
  iVar2 = (**(code **)(*param_1 + 300))(param_1,(int)cVar1);
  param_1[5] = iVar2;
  return;
}

