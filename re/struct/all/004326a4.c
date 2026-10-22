
void FUN_004326a4(int param_1,int param_2,undefined4 *param_3)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 local_48 [15];
  
  puVar2 = local_48;
  for (iVar1 = 0xf; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar2 = *param_3;
    param_3 = param_3 + 1;
    puVar2 = puVar2 + 1;
  }
  FUN_0043222c(param_1,*(undefined4 *)(param_2 + 0xc),
               *(int *)(*(int *)(param_1 + 0x80) + 0x84) + *(int *)(param_2 + 0x84),local_48,
               *(int *)(*(int *)(param_1 + 0x80) + 0x88) + *(int *)(param_2 + 0x88));
  return;
}

