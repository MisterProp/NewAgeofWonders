
void FUN_00412c3c(undefined4 param_1,int param_2,undefined4 *param_3)

{
  int *piVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 local_48 [15];
  
  puVar3 = local_48;
  for (iVar2 = 0xf; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar3 = *param_3;
    param_3 = param_3 + 1;
    puVar3 = puVar3 + 1;
  }
  piVar1 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                            (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0xa8),
                             *(undefined4 *)(param_2 + 0xc),0,local_48);
  (**(code **)(*piVar1 + 0x84))(piVar1,*(int *)(param_2 + 0x84) + 5,*(int *)(param_2 + 0x88) + 5);
  return;
}

