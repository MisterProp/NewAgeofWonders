
void ActivityPreDrawCity(int param_1,int param_2,undefined4 *param_3)

{
  undefined4 uVar1;
  int *piVar2;
  int iVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  undefined4 local_4c [15];
  
  puVar5 = local_4c;
  puVar4 = local_4c;
  for (iVar3 = 0xf; iVar3 != 0; iVar3 = iVar3 + -1) {
    *puVar4 = *param_3;
    param_3 = param_3 + 1;
    puVar4 = puVar4 + 1;
  }
  uVar1 = FUN_0043d290(param_1,CONCAT31((int3)((uint)param_2 >> 8),*(undefined1 *)(param_2 + 0xc)) +
                               1);
  piVar2 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                            (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0xec),uVar1);
  (**(code **)(*piVar2 + 0x84))
            (piVar2,*(int *)(*(int *)(param_1 + 0x148) + 0x84) + *(int *)(param_2 + 0x84) + 2,
             *(int *)(*(int *)(param_1 + 0x148) + 0x88) + *(int *)(param_2 + 0x88) + 2,puVar5);
  return;
}

