
void FUN_00433260(int param_1,int param_2,undefined4 *param_3)

{
  undefined1 *puVar1;
  int iVar2;
  uint uVar3;
  undefined4 *puVar4;
  int local_a0;
  undefined1 local_9c [4];
  undefined4 local_98 [11];
  undefined1 auStack_6c [12];
  undefined1 auStack_60 [60];
  undefined1 auStack_24 [4];
  int local_20;
  int local_1c;
  int local_18;
  int local_14;
  
  puVar4 = local_98;
  for (iVar2 = 0xf; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar4 = *param_3;
    param_3 = param_3 + 1;
    puVar4 = puVar4 + 1;
  }
  Classes_Rect_9923E97E
            (*(int *)(*(int *)(param_1 + 0x58) + 0x84) + *(int *)(param_2 + 0x84) + 1,
             *(int *)(*(int *)(param_1 + 0x58) + 0x88) + *(int *)(param_2 + 0x88) + 1,
             *(int *)(*(int *)(param_1 + 0x58) + 0x84) + *(int *)(param_2 + 0x84) +
             *(int *)(param_2 + 0x7c) + -2,&local_20,
             *(int *)(*(int *)(param_1 + 0x58) + 0x88) + *(int *)(param_2 + 0x88) +
             *(int *)(param_2 + 0x80) + -2);
  uVar3 = local_18 - local_20;
  puVar1 = local_9c;
  HSEngine_HXtoHP_8E9B8701((int)*(char *)(param_1 + 0xac),(int)*(char *)(param_1 + 0xad),&local_a0);
  puVar1 = puVar1 + (0x10 - (uVar3 >> 1));
  local_a0 = (local_a0 + 0x10) - ((uint)(local_14 - local_1c) >> 1);
  if ((int)puVar1 < 0x10) {
    puVar1 = (undefined1 *)0x10;
  }
  if (local_a0 < 0x10) {
    local_a0 = 0x10;
  }
  GFXE_CreateSDfromSD_1AF820EE(local_9c,auStack_24,auStack_60);
  HSEngine_THSMap_SetViewPort_23EDC2EF
            (*(undefined4 *)AoWE_AoWHSMap_6B957A58_exref,puVar1,local_a0,local_18 - local_20,uVar3,
             (int)*(char *)(param_1 + 0xae));
  (**(code **)(**(int **)AoWE_AoWHSMap_6B957A58_exref + 0xd8))();
  (**(code **)(**(int **)AoWE_AoWHSMap_6B957A58_exref + 0xdc))();
  (**(code **)(**(int **)AoWE_AoWHSMap_6B957A58_exref + 0xe0))
            (*(int **)AoWE_AoWHSMap_6B957A58_exref,auStack_6c);
  (**(code **)(**(int **)AoWE_AoWHSMap_6B957A58_exref + 0xe4))
            (*(int **)AoWE_AoWHSMap_6B957A58_exref,auStack_6c);
  return;
}

