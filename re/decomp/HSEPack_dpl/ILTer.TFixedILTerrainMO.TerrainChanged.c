
void ILTer_TFixedILTerrainMO_TerrainChanged_23EDC2EF(int *param_1,char *param_2)

{
  char cVar1;
  int iVar2;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_20;
  undefined1 *puStack_1c;
  undefined1 *puStack_18;
  
                    /* 0x18d58  937  ILTer.TFixedILTerrainMO.TerrainChanged@23EDC2EF */
  puStack_18 = (undefined1 *)0x55618d76;
  cVar1 = HSEngine_TTerrainList_GetTerrain_23EDC2EF
                    (*(undefined4 *)(param_1[2] + 0x28),(int)param_2[8]);
  if (-1 < cVar1) {
    puStack_18 = (undefined1 *)0x55618d8c;
    cVar1 = (**(code **)(*param_1 + 0x128))(param_1,(int)*param_2);
    if (cVar1 == '\0') {
      puStack_1c = &LAB_55618dfc;
      uStack_20 = *in_FS_OFFSET;
      *in_FS_OFFSET = &uStack_20;
      puStack_18 = &stack0xfffffffc;
      SysUtils_Exception_Create_23EDC2EF
                (SysUtils__Exception_9880D0BF_exref,1,"TILFixedTerrain Failure");
      System__RaiseExcept_51F89FF7();
      *in_FS_OFFSET = uStack_20;
    }
    else {
      *(char *)(param_1 + 6) = *param_2;
      puStack_18 = (undefined1 *)(int)*(char *)((int)param_1 + 0x11);
      puStack_1c = (undefined1 *)0x55618db8;
      HSEngine_TMultiHexMO_UpdateTerrain_23EDC2EF
                (param_1,*(undefined4 *)(param_1[1] + 4),(int)(char)param_1[4]);
      puStack_18 = (undefined1 *)0x55618dc6;
      iVar2 = (**(code **)(*param_1 + 300))(param_1,(int)*param_2);
      param_1[5] = iVar2;
    }
  }
  return;
}

