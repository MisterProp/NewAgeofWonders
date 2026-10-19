
undefined4
Mountain_TMountainMO_CanPlace_23EDC2EF
          (int param_1,int param_2,char param_3,char param_4,char param_5)

{
  int iVar1;
  char cVar2;
  byte bVar3;
  char cVar4;
  int *piVar5;
  int iVar6;
  undefined4 uVar7;
  int iVar8;
  int local_1c;
  int local_18;
  undefined4 local_14;
  char local_d;
  int local_c;
  int local_8;
  
  cVar4 = '\x01';
  iVar1 = *(int *)(*(int *)(param_1 + 8) + 0x28);
  local_d = param_3;
  local_c = param_2;
  local_8 = param_1;
                    /* 0xa2e54  6619  Mountain.TMountainMO.CanPlace@23EDC2EF */
  for (iVar8 = 0; (cVar4 != '\0' && (iVar8 < *(int *)(iVar1 + 4))); iVar8 = iVar8 + 1) {
    cVar2 = HSEngine_TTerrainList_GetTerrain_23EDC2EF(iVar1,iVar8);
    if (((-1 < cVar2) ||
        (cVar2 = HSEngine_TTerrainList_GetOverlay_23EDC2EF(iVar1,iVar8), -1 < cVar2)) &&
       ((((HSEngine_CenterHNtoHX_A52F39E4((int)local_d,(int)param_5,iVar8,&local_1c,&local_18),
          -1 < local_18 && (-1 < local_1c)) && (local_18 < *(int *)(local_c + 0xc))) &&
        (local_1c < *(int *)(local_c + 0x10))))) {
      local_14 = HSEngine_TMapContainer_GetField_23EDC2EF(local_c,local_18,local_1c,(int)param_4);
      piVar5 = (int *)HSEngine_TMapField_GetMaxLevelNoneTransparentHS_23EDC2EF(local_14);
      if ((piVar5 == (int *)0x0) || (iVar6 = (**(code **)(*piVar5 + 0x24))(), iVar6 != 0x20168)) {
        if ((piVar5 == (int *)0x0) || (bVar3 = (**(code **)(*piVar5 + 100))(), bVar3 < 10)) {
          cVar4 = '\x01';
        }
        else {
          cVar4 = '\0';
        }
      }
      else {
        uVar7 = HSEngine_TTerrainList_GetOverlay_23EDC2EF(iVar1,iVar8);
        cVar4 = Mountain_TMountainMO_ValidMountainOverlap_23EDC2EF(local_8,local_14,uVar7);
      }
    }
  }
  if ((cVar4 == '\0') ||
     (cVar4 = HSEngine_THSMap_CanChangeTerrain_23EDC2EF
                        (*(undefined4 *)(local_c + 0x18),local_8,(int)local_d,
                         *(undefined4 *)(*(int *)(local_8 + 8) + 0x28),(int)param_4,(int)param_5),
     cVar4 == '\0')) {
    uVar7 = 0;
  }
  else {
    uVar7 = 1;
  }
  return uVar7;
}

