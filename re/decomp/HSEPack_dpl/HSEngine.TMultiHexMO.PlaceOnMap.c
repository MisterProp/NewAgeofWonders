
undefined4 HSEngine_TMultiHexMO_PlaceOnMap_23EDC2EF(int *param_1)

{
  int iVar1;
  byte bVar2;
  char cVar3;
  int iVar4;
  uint uVar5;
  undefined4 uVar6;
  undefined1 auStack_2c [8];
  int iStack_24;
  int iStack_1c;
  int iStack_18;
  
                    /* 0xacec  209  HSEngine.TMultiHexMO.PlaceOnMap@23EDC2EF */
  if ((*(byte *)(param_1 + 3) & 8) == 0) {
    (**(code **)(*param_1 + 0x90))();
    iVar4 = (**(code **)(*param_1 + 0x54))();
    iVar1 = *(int *)(param_1[1] + 4);
    bVar2 = (**(code **)(*param_1 + 100))();
    if (bVar2 < 0x10) {
      HSEngine_InitHNWalk_3DC7E9B4
                (auStack_2c,(int)(char)param_1[4],(int)*(char *)((int)param_1 + 0x11),0);
      while (iStack_24 < iVar4) {
        cVar3 = (**(code **)(*param_1 + 0x4c))(param_1,iStack_24);
        if (cVar3 < -1) {
          cVar3 = (**(code **)(*param_1 + 0x58))(param_1,iStack_24);
          if (-2 < cVar3) goto LAB_5560ad55;
        }
        else {
LAB_5560ad55:
          if ((((-1 < iStack_1c) && (-1 < iStack_18)) && (iStack_1c < *(int *)(iVar1 + 0xc))) &&
             (iStack_18 < *(int *)(iVar1 + 0x10))) {
            HSEngine_THSMap_InvalidateStaticScene_23EDC2EF
                      (*(undefined4 *)(iVar1 + 0x1c),iStack_1c,iStack_18,
                       (int)*(char *)(param_1[1] + 0x12));
          }
        }
        HSEngine_WalkToNextHN_77BE26A7(auStack_2c);
      }
    }
    cVar3 = (**(code **)(*param_1 + 0x84))();
    if (cVar3 == '\0') {
      uVar5 = (uint)*(byte *)(iVar1 + 8);
      uVar6 = HSEngine_HNtoRad_DB882642(iVar4);
      HSEngine_THSMap_TerrainChangedRad_23EDC2EF
                (*(undefined4 *)(iVar1 + 0x1c),(char)param_1[4],*(undefined1 *)((int)param_1 + 0x11)
                 ,uVar6,uVar5);
    }
  }
  return 1;
}

