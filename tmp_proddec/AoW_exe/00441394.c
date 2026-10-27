// /AoW.exe : 00441394

void MapViewDraw(int param_1,int param_2,undefined4 *param_3)

{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  int iVar4;
  undefined4 local_98 [12];
  undefined1 auStack_68 [12];
  undefined1 auStack_5c [60];
  int local_20;
  int local_1c;
  int local_18;
  int local_14;
  
  puVar3 = local_98;
  for (iVar1 = 0xf; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar3 = *param_3;
    param_3 = param_3 + 1;
    puVar3 = puVar3 + 1;
  }
  if (*(int *)(param_1 + 0x1f8) != 0) {
    Classes_Rect_9923E97E
              (*(int *)(*(int *)(param_1 + 0x44) + 0x84) + *(int *)(param_2 + 0x84),
               *(int *)(*(int *)(param_1 + 0x44) + 0x88) + *(int *)(param_2 + 0x88),
               *(int *)(*(int *)(param_1 + 0x44) + 0x84) + *(int *)(param_2 + 0x84) +
               *(int *)(param_2 + 0x7c),&local_20,
               *(int *)(*(int *)(param_1 + 0x44) + 0x88) + *(int *)(param_2 + 0x88) +
               *(int *)(param_2 + 0x80));
    iVar2 = local_18 - local_20;
    iVar4 = local_14 - local_1c;
    iVar1 = HSEngine_THSMap_GetMaxXhp_23EDC2EF(*(undefined4 *)(param_1 + 0x1f8));
    if (iVar1 < *(int *)(param_1 + 0x1e0) + iVar2 + 0x10) {
      iVar1 = HSEngine_THSMap_GetMaxXhp_23EDC2EF(*(undefined4 *)(param_1 + 0x1f8));
      *(int *)(param_1 + 0x1e0) = (iVar1 - iVar2) + -0x10;
    }
    iVar1 = HSEngine_THSMap_GetMaxYhp_23EDC2EF(*(undefined4 *)(param_1 + 0x1f8));
    if (iVar1 < *(int *)(param_1 + 0x1e4) + iVar4 + 0x10) {
      iVar1 = HSEngine_THSMap_GetMaxYhp_23EDC2EF(*(undefined4 *)(param_1 + 0x1f8));
      *(int *)(param_1 + 0x1e4) = (iVar1 - iVar4) + -0x10;
    }
    if (*(int *)(param_1 + 0x1e0) < 0x10) {
      *(undefined4 *)(param_1 + 0x1e0) = 0x10;
    }
    if (*(int *)(param_1 + 0x1e4) < 0x10) {
      *(undefined4 *)(param_1 + 0x1e4) = 0x10;
    }
    GFXE_CreateSDfromSD_1AF820EE(local_98,&local_20,auStack_5c);
    (**(code **)(**(int **)(param_1 + 0x1f8) + 0x104))(*(int **)(param_1 + 0x1f8),0);
    (**(code **)(**(int **)(param_1 + 0x1f8) + 0x100))(*(int **)(param_1 + 0x1f8),0);
    HSEngine_THSMap_SetViewPort_23EDC2EF
              (*(undefined4 *)(param_1 + 0x1f8),*(undefined4 *)(param_1 + 0x1e0),
               *(undefined4 *)(param_1 + 0x1e4),local_14 - local_1c,iVar2,0);
    (**(code **)(**(int **)(param_1 + 0x1f8) + 0xd8))();
    (**(code **)(**(int **)(param_1 + 0x1f8) + 0xdc))();
    (**(code **)(**(int **)(param_1 + 0x1f8) + 0xe0))(*(int **)(param_1 + 0x1f8),auStack_68);
    (**(code **)(**(int **)(param_1 + 0x1f8) + 0xe4))(*(int **)(param_1 + 0x1f8),auStack_68);
  }
  return;
}

