
void FUN_0040a214(undefined4 param_1,int param_2,undefined4 *param_3)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 local_5c [15];
  undefined1 auStack_20 [16];
  
  puVar2 = local_5c;
  for (iVar1 = 0xf; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar2 = *param_3;
    param_3 = param_3 + 1;
    puVar2 = puVar2 + 1;
  }
  if (*(int **)(*(int *)PTR_DAT_0045a420 + 0xc0) != (int *)0x0) {
    iVar1 = (**(code **)(**(int **)(*(int *)PTR_DAT_0045a420 + 0xc0) + 0xf4))();
    if (iVar1 != 0) {
      Classes_Rect_9923E97E
                (*(int *)(param_2 + 0x84) + 0x1d,*(int *)(param_2 + 0x88) + 0x1c,
                 *(int *)(param_2 + 0x84) + 0x84,auStack_20,*(int *)(param_2 + 0x88) + 0x9c);
      AoWE_TAbstractUnit_ShowFace_23EDC2EF
                (*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0xc0),auStack_20,local_5c);
    }
  }
  return;
}

