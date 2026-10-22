
void FUN_00450ef8(int param_1,undefined4 param_2,undefined1 param_3,int param_4,int param_5)

{
  int iVar1;
  int iVar2;
  undefined2 extraout_var;
  
  iVar1 = *(int *)(param_1 + 0x44);
  if ((((*(int *)(iVar1 + 0x84) + 0xb < param_5) &&
       (param_5 < *(int *)(*(int *)(param_1 + 0x44) + 0x84) + *(int *)(iVar1 + 0x7c) + -0xb)) &&
      (*(int *)(iVar1 + 0x88) + 0xb < param_4)) &&
     (iVar2 = *(int *)(iVar1 + 0x88) + *(int *)(iVar1 + 0x80) + -0xb, param_4 < iVar2)) {
    (**(code **)(**(int **)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xc) + 0x20))
              (*(int **)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xc),
               CONCAT31((int3)((uint)iVar2 >> 8),param_3),
               (param_5 + -0xc) - *(int *)(*(int *)(param_1 + 0x44) + 0x84),
               (param_4 + -0xc) - *(int *)(iVar1 + 0x88));
    AoWWinGeneric_TAOWGenericWindow_SetCursor_23EDC2EF
              (*(undefined4 *)(param_1 + 0x44),CONCAT22(extraout_var,9999));
  }
  return;
}

