
void ILTer_TILTerrainMO_Show_23EDC2EF(int param_1,int param_2,int param_3,undefined4 param_4)

{
  int iVar1;
  int *piVar2;
  int local_10;
  int local_c;
  int local_8;
  
                    /* 0x18940  951  ILTer.TILTerrainMO.Show@23EDC2EF */
  local_8 = param_3;
  HSEngine_HXtoHP_8E9B8701
            ((int)*(char *)(param_1 + 0x10),(int)*(char *)(param_1 + 0x11),&local_c,&local_10);
  iVar1 = *(int *)(param_1 + 8);
  if (*(int *)(iVar1 + 0x24) == 0) {
    piVar2 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                              (*(undefined4 *)(iVar1 + 0x20),*(undefined4 *)(param_1 + 0x14));
    (**(code **)(*piVar2 + 0x84))
              (piVar2,(local_c - param_2) + *(int *)(*(int *)(param_1 + 8) + 0x34),
               (local_10 - local_8) + *(int *)(*(int *)(param_1 + 8) + 0x38),param_4);
  }
  else {
    HSEngine_TMapObjectResource_ShowComplexImage_23EDC2EF
              (iVar1,local_c - param_2,local_10 - local_8,local_c + local_10,
               *(undefined4 *)(param_1 + 0x14),param_4);
  }
  return;
}

