
void FUN_004264c8(int param_1,int param_2,undefined4 *param_3)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int *piVar3;
  int iVar4;
  char cVar5;
  undefined4 *puVar6;
  undefined1 **ppuVar7;
  undefined1 *puStack_60;
  undefined4 local_5c [14];
  undefined1 auStack_24 [4];
  undefined1 auStack_20 [16];
  
  puVar6 = local_5c;
  for (iVar4 = 0xf; iVar4 != 0; iVar4 = iVar4 + -1) {
    *puVar6 = *param_3;
    param_3 = param_3 + 1;
    puVar6 = puVar6 + 1;
  }
  if (*(int *)(param_2 + 0xc) < *(int *)(*(int *)(param_1 + 0x288) + 8)) {
    puStack_60 = (undefined1 *)0x4264fc;
    uVar1 = Engine_TIntegerList_Get_23EDC2EF(*(int *)(param_1 + 0x288),*(int *)(param_2 + 0xc));
    puStack_60 = (undefined1 *)0x42651e;
    uVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                       (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
    puStack_60 = (undefined1 *)0x426525;
    iVar4 = AoWE_TPlayer_GetFirstDaySeen_23EDC2EF(uVar2,uVar1);
    cVar5 = (char)uVar1;
    if (iVar4 == -1) {
      puStack_60 = (undefined1 *)local_5c;
      uVar1 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                        (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0x98),0x1c);
      ImageLib_TLibraryImage_ShowClipped_23EDC2EF
                (uVar1,*(undefined4 *)(param_2 + 0x84),*(undefined4 *)(param_2 + 0x88));
    }
    else {
      puStack_60 = (undefined1 *)(*(int *)(param_2 + 0x88) + *(int *)(param_2 + 0x80) + -1);
      Classes_Rect_9923E97E
                (*(int *)(param_2 + 0x84) + 1,*(int *)(param_2 + 0x88) + 1,
                 *(int *)(param_2 + 0x84) + *(int *)(param_2 + 0x7c) + -1,auStack_20);
      puStack_60 = (undefined1 *)0x426596;
      AoWE_TPlayerList_GetPlayers_23EDC2EF
                (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),(int)cVar5);
      puStack_60 = (undefined1 *)0x42659b;
      iVar4 = AoWE_TPlayer_GetFace_23EDC2EF();
      if (iVar4 != 0) {
        AoWE_TPlayerList_GetPlayers_23EDC2EF
                  (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),(int)cVar5);
        piVar3 = (int *)AoWE_TPlayer_GetFace_23EDC2EF();
        (**(code **)(*piVar3 + 0x88))(piVar3,auStack_24,&puStack_60);
      }
    }
    ppuVar7 = &puStack_60;
    iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),(int)cVar5);
    uVar1 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                      (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0x98),
                       *(char *)(iVar4 + 0xa5) + 0x10);
    ImageLib_TLibraryImage_ShowClipped_23EDC2EF
              (uVar1,*(undefined4 *)(param_2 + 0x84),*(undefined4 *)(param_2 + 0x88),ppuVar7);
  }
  return;
}

