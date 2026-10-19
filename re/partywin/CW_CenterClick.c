
void FUN_0040674c(undefined4 param_1,int param_2,undefined4 *param_3)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  undefined4 *puVar4;
  byte bVar5;
  undefined4 local_6c [4];
  undefined4 local_5c [11];
  undefined4 local_30 [4];
  undefined1 local_20 [16];
  
  bVar5 = 0;
  puVar4 = local_6c;
  puVar2 = local_6c;
  for (iVar1 = 0xf; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar2 = *param_3;
    param_3 = param_3 + 1;
    puVar2 = puVar2 + 1;
  }
  if (*(int *)(*(int *)PTR_DAT_0045a420 + 0xa4) != 0) {
    puVar2 = local_5c;
    puVar3 = local_30;
    for (iVar1 = 4; iVar1 != 0; iVar1 = iVar1 + -1) {
      *puVar3 = *puVar2;
      puVar2 = puVar2 + 1;
      puVar3 = puVar3 + 1;
    }
    iVar1 = *(int *)(*(int *)(param_2 + 0xcc) + 0x84) + *(int *)(param_2 + 0x84);
    Classes_Rect_9923E97E
              (iVar1 + 4,*(int *)(*(int *)(param_2 + 0xcc) + 0x88) + *(int *)(param_2 + 0x88) + 3,
               iVar1 + *(int *)(param_2 + 0x7c) + -3,local_20,
               *(int *)(*(int *)(param_2 + 0xcc) + 0x88) + *(int *)(param_2 + 0x88) +
               *(int *)(param_2 + 0x80) + -7);
    FUN_00405d98(param_1,*(undefined4 *)(param_2 + 0xc),local_20,puVar4);
    puVar2 = local_30;
    puVar4 = local_5c;
    for (iVar1 = 4; iVar1 != 0; iVar1 = iVar1 + -1) {
      *puVar4 = *puVar2;
      puVar2 = puVar2 + (uint)bVar5 * -2 + 1;
      puVar4 = puVar4 + (uint)bVar5 * -2 + 1;
    }
  }
  return;
}

