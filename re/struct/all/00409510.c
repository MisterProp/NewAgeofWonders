
void FUN_00409510(int param_1,int param_2)

{
  char cVar1;
  int iVar2;
  int *piVar3;
  undefined4 uVar4;
  int iVar5;
  
  *(undefined4 *)(*(int *)(param_1 + 0x148) + 0xc) = 0xffffffff;
  *(undefined4 *)(*(int *)(param_1 + 0x144) + 0xc) = 0xffffffff;
  *(undefined4 *)(*(int *)(param_1 + 0x140) + 0xc) = 0xffffffff;
  *(undefined4 *)(*(int *)(param_1 + 0x14c) + 0xc) = 0xffffffff;
  *(undefined4 *)(*(int *)(param_1 + 0x150) + 0xc) = 0xffffffff;
  (**(code **)(**(int **)(param_1 + 0x148) + 0xac))(*(int **)(param_1 + 0x148),0);
  (**(code **)(**(int **)(param_1 + 0x144) + 0xac))(*(int **)(param_1 + 0x144),0);
  (**(code **)(**(int **)(param_1 + 0x140) + 0xac))(*(int **)(param_1 + 0x140),0);
  (**(code **)(**(int **)(param_1 + 0x14c) + 0xac))(*(int **)(param_1 + 0x14c),0);
  (**(code **)(**(int **)(param_1 + 0x150) + 0xac))(*(int **)(param_1 + 0x150),0);
  iVar5 = *(int *)(*(int *)PTR_DAT_0045a420 + 0xc0);
  if (iVar5 != 0) {
    cVar1 = System__IsClass_51F89FF7(iVar5,AoWE__THero_228E24B5_exref);
    if (cVar1 != '\0') {
      iVar2 = AoWE_THero_GetItemsOnGround_23EDC2EF(iVar5);
      if (iVar2 != 0) {
        piVar3 = (int *)AoWE_THero_GetItemsOnGround_23EDC2EF(iVar5);
        uVar4 = (**(code **)(*piVar3 + 0x54))();
        *(undefined4 *)(param_1 + 0x238) = uVar4;
        *(int *)(param_1 + 0x234) = param_2;
        iVar2 = *(int *)(param_1 + 0x238);
        if (iVar2 < param_2 + 5) {
          iVar2 = iVar2 + -4;
          *(int *)(param_1 + 0x234) = iVar2;
        }
        if (*(int *)(param_1 + 0x234) < 0) {
          *(undefined4 *)(param_1 + 0x234) = 0;
        }
        (**(code **)(**(int **)(param_1 + 0x1e4) + 0x6c))
                  (*(int **)(param_1 + 0x1e4),
                   CONCAT31((int3)((uint)iVar2 >> 8),0 < *(int *)(param_1 + 0x234)));
        (**(code **)(**(int **)(param_1 + 0x1e8) + 0x6c))
                  (*(int **)(param_1 + 0x1e8),
                   *(int *)(param_1 + 0x234) + 5 <= *(int *)(param_1 + 0x238));
        piVar3 = (int *)AoWE_THero_GetItemsOnGround_23EDC2EF(iVar5);
        iVar2 = (**(code **)(*piVar3 + 0x54))();
        if (*(int *)(param_1 + 0x234) < iVar2) {
          piVar3 = (int *)AoWE_THero_GetItemsOnGround_23EDC2EF(iVar5);
          iVar2 = (**(code **)(*piVar3 + 0x8c))(piVar3,*(undefined4 *)(param_1 + 0x234));
          *(undefined4 *)(*(int *)(param_1 + 0x148) + 0xc) = *(undefined4 *)(iVar2 + 0x14);
        }
        piVar3 = (int *)AoWE_THero_GetItemsOnGround_23EDC2EF(iVar5);
        iVar2 = (**(code **)(*piVar3 + 0x54))();
        if (*(int *)(param_1 + 0x234) + 1 < iVar2) {
          piVar3 = (int *)AoWE_THero_GetItemsOnGround_23EDC2EF(iVar5);
          iVar2 = (**(code **)(*piVar3 + 0x8c))(piVar3,*(int *)(param_1 + 0x234) + 1);
          *(undefined4 *)(*(int *)(param_1 + 0x144) + 0xc) = *(undefined4 *)(iVar2 + 0x14);
        }
        piVar3 = (int *)AoWE_THero_GetItemsOnGround_23EDC2EF(iVar5);
        iVar2 = (**(code **)(*piVar3 + 0x54))();
        if (*(int *)(param_1 + 0x234) + 2 < iVar2) {
          piVar3 = (int *)AoWE_THero_GetItemsOnGround_23EDC2EF(iVar5);
          iVar2 = (**(code **)(*piVar3 + 0x8c))(piVar3,*(int *)(param_1 + 0x234) + 2);
          *(undefined4 *)(*(int *)(param_1 + 0x140) + 0xc) = *(undefined4 *)(iVar2 + 0x14);
        }
        piVar3 = (int *)AoWE_THero_GetItemsOnGround_23EDC2EF(iVar5);
        iVar2 = (**(code **)(*piVar3 + 0x54))();
        if (*(int *)(param_1 + 0x234) + 3 < iVar2) {
          piVar3 = (int *)AoWE_THero_GetItemsOnGround_23EDC2EF(iVar5);
          iVar2 = (**(code **)(*piVar3 + 0x8c))(piVar3,*(int *)(param_1 + 0x234) + 3);
          *(undefined4 *)(*(int *)(param_1 + 0x14c) + 0xc) = *(undefined4 *)(iVar2 + 0x14);
        }
        piVar3 = (int *)AoWE_THero_GetItemsOnGround_23EDC2EF(iVar5);
        iVar2 = (**(code **)(*piVar3 + 0x54))();
        if (*(int *)(param_1 + 0x234) + 4 < iVar2) {
          piVar3 = (int *)AoWE_THero_GetItemsOnGround_23EDC2EF(iVar5);
          iVar5 = (**(code **)(*piVar3 + 0x8c))(piVar3,*(int *)(param_1 + 0x234) + 4);
          *(undefined4 *)(*(int *)(param_1 + 0x150) + 0xc) = *(undefined4 *)(iVar5 + 0x14);
        }
      }
    }
  }
  return;
}

