
void FUN_0045431c(int param_1)

{
  int *piVar1;
  undefined *puVar2;
  undefined *puVar3;
  int iVar4;
  uint uVar5;
  undefined4 uVar6;
  undefined4 *puVar7;
  undefined4 *puVar8;
  byte bVar9;
  undefined4 auStack_20 [4];
  
  puVar3 = PTR_DAT_0045a5bc;
  puVar2 = PTR_DAT_0045a1c0;
  bVar9 = 0;
  piVar1 = *(int **)(*(int *)PTR_DAT_0045a1c0 + 0x44);
  iVar4 = piVar1[0x35];
  *(undefined4 *)(iVar4 + 0x20) = 0;
  *(undefined4 *)(iVar4 + 0x1c) = 0;
  iVar4 = *(int *)(*(int *)(DAT_0045b2c0 + 0x44) + 0x80);
  if (iVar4 < 0x2bd) {
    uVar5 = iVar4 - *(int *)(*(int *)(*(int *)PTR_DAT_0045a0cc + 0x44) + 0x80);
    iVar4 = (int)uVar5 >> 1;
    if (iVar4 < 0) {
      iVar4 = iVar4 + (uint)((uVar5 & 1) != 0);
    }
    (**(code **)(*piVar1 + 100))(piVar1,iVar4);
  }
  else {
    (**(code **)(*piVar1 + 100))
              (piVar1,(iVar4 - *(int *)(*(int *)(*(int *)PTR_DAT_0045a0cc + 0x44) + 0x80)) / 3);
  }
  (**(code **)(*piVar1 + 0x60))(piVar1,(piVar1[0x20] * 200) / 0xd8);
  if (*(int *)AoWE_AoWHSMap_6B957A58_exref != 0) {
    if ((*(int *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x11c) == 0) &&
       (*(char *)(*(int *)(*(int *)puVar3 + 0x44) + 0x75) != '\0')) {
      uVar6 = 1;
    }
    else {
      uVar6 = 0;
    }
    (**(code **)(*piVar1 + 0x6c))(piVar1,uVar6);
  }
  piVar1 = *(int **)(*(int *)PTR_DAT_0045a4fc + 0x44);
  iVar4 = piVar1[0x35];
  *(undefined4 *)(iVar4 + 0x24) = 0;
  *(undefined4 *)(iVar4 + 0x1c) = 0;
  (**(code **)(*piVar1 + 0x60))(piVar1,*(undefined4 *)(*(int *)(*(int *)puVar2 + 0x44) + 0x7c));
  (**(code **)(*piVar1 + 100))(piVar1,*(undefined4 *)(*(int *)(*(int *)puVar2 + 0x44) + 0x80));
  if (*(int *)AoWE_AoWHSMap_6B957A58_exref != 0) {
    if ((*(int *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x11c) == 0) &&
       (*(char *)(*(int *)(*(int *)puVar3 + 0x44) + 0x75) != '\0')) {
      uVar6 = 1;
    }
    else {
      uVar6 = 0;
    }
    (**(code **)(*piVar1 + 0x6c))(piVar1,uVar6);
  }
  piVar1 = *(int **)(*(int *)PTR_DAT_0045a084 + 0x44);
  iVar4 = piVar1[0x35];
  *(undefined4 *)(iVar4 + 0x20) = *(undefined4 *)(*(int *)(*(int *)puVar2 + 0x44) + 0x80);
  *(undefined4 *)(iVar4 + 0x1c) = 0;
  (**(code **)(*piVar1 + 0x60))(piVar1,*(undefined4 *)(*(int *)(*(int *)puVar2 + 0x44) + 0x7c));
  (**(code **)(*piVar1 + 100))(piVar1,*(undefined4 *)(*(int *)(*(int *)puVar2 + 0x44) + 0x80));
  (**(code **)(*piVar1 + 0x6c))(piVar1,0);
  piVar1 = *(int **)(*(int *)PTR_DAT_0045a5c4 + 0x44);
  iVar4 = piVar1[0x35];
  *(undefined4 *)(iVar4 + 0x20) = *(undefined4 *)(*(int *)(*(int *)puVar2 + 0x44) + 0x80);
  *(undefined4 *)(iVar4 + 0x1c) = 0;
  (**(code **)(*piVar1 + 0x60))(piVar1,*(undefined4 *)(*(int *)(*(int *)puVar2 + 0x44) + 0x7c));
  (**(code **)(*piVar1 + 100))(piVar1,*(undefined4 *)(*(int *)(*(int *)puVar2 + 0x44) + 0x80));
  (**(code **)(*piVar1 + 0x6c))(piVar1,0);
  piVar1 = *(int **)(*(int *)PTR_DAT_0045a44c + 0x44);
  iVar4 = piVar1[0x35];
  *(undefined4 *)(iVar4 + 0x20) = *(undefined4 *)(*(int *)(*(int *)puVar2 + 0x44) + 0x80);
  *(undefined4 *)(iVar4 + 0x1c) = 0;
  (**(code **)(*piVar1 + 0x60))(piVar1,*(undefined4 *)(*(int *)(*(int *)puVar2 + 0x44) + 0x7c));
  (**(code **)(*piVar1 + 100))(piVar1,*(undefined4 *)(*(int *)(*(int *)puVar2 + 0x44) + 0x80));
  piVar1 = *(int **)(*(int *)PTR_DAT_0045a0b0 + 0x44);
  iVar4 = piVar1[0x35];
  *(undefined4 *)(iVar4 + 0x24) = *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a0cc + 0x44) + 0x80);
  *(undefined4 *)(iVar4 + 0x18) = 0;
  *(undefined4 *)(iVar4 + 0x1c) = *(undefined4 *)(*(int *)(*(int *)puVar2 + 0x44) + 0x7c);
  *(undefined1 *)(iVar4 + 0x28) = 3;
  *(undefined1 *)(iVar4 + 0x29) = 2;
  (**(code **)(*piVar1 + 0x60))(piVar1,piVar1[0x55]);
  (**(code **)(*piVar1 + 100))(piVar1,piVar1[0x56]);
  (**(code **)(*piVar1 + 0x6c))(piVar1,0);
  piVar1 = *(int **)(*(int *)PTR_DAT_0045a394 + 0x44);
  iVar4 = piVar1[0x35];
  *(undefined4 *)(iVar4 + 0x24) = *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a0cc + 0x44) + 0x80);
  *(undefined4 *)(iVar4 + 0x18) = 0;
  *(undefined4 *)(iVar4 + 0x1c) = *(undefined4 *)(*(int *)(*(int *)puVar2 + 0x44) + 0x7c);
  *(undefined1 *)(iVar4 + 0x28) = 3;
  *(undefined1 *)(iVar4 + 0x29) = 2;
  (**(code **)(*piVar1 + 0x60))(piVar1,piVar1[0x55]);
  (**(code **)(*piVar1 + 100))(piVar1,piVar1[0x56]);
  (**(code **)(*piVar1 + 0x6c))(piVar1,0);
  piVar1 = *(int **)(*(int *)PTR_DAT_0045a53c + 0x44);
  iVar4 = piVar1[0x35];
  *(undefined1 *)(iVar4 + 0x28) = 4;
  *(undefined1 *)(iVar4 + 0x29) = 4;
  (**(code **)(*piVar1 + 0x60))(piVar1,piVar1[0x55]);
  (**(code **)(*piVar1 + 100))(piVar1,piVar1[0x56]);
  (**(code **)(*piVar1 + 0x6c))(piVar1,0);
  piVar1 = *(int **)(*(int *)PTR_DAT_0045a490 + 0x44);
  iVar4 = piVar1[0x35];
  *(undefined4 *)(iVar4 + 0x24) = *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a0cc + 0x44) + 0x80);
  *(undefined4 *)(iVar4 + 0x18) = 0;
  *(undefined4 *)(iVar4 + 0x1c) = *(undefined4 *)(*(int *)(*(int *)puVar2 + 0x44) + 0x7c);
  *(undefined1 *)(iVar4 + 0x28) = 3;
  *(undefined1 *)(iVar4 + 0x29) = 2;
  (**(code **)(*piVar1 + 0x60))(piVar1,piVar1[0x55]);
  (**(code **)(*piVar1 + 100))(piVar1,piVar1[0x56]);
  (**(code **)(*piVar1 + 0x6c))(piVar1,0);
  piVar1 = *(int **)(*(int *)PTR_DAT_0045a098 + 0x44);
  iVar4 = piVar1[0x35];
  *(undefined4 *)(iVar4 + 0x20) = 0;
  *(undefined4 *)(iVar4 + 0x1c) = 0;
  (**(code **)(*piVar1 + 100))
            (piVar1,((*(int *)(*(int *)(DAT_0045b2c0 + 0x44) + 0x80) -
                     *(int *)(*(int *)(*(int *)PTR_DAT_0045a0cc + 0x44) + 0x80)) * 0xaa) / 0x1b0);
  (**(code **)(*piVar1 + 0x60))(piVar1,(piVar1[0x20] * 0x96) / 0xaa);
  if (*(int *)AoWE_AoWHSMap_6B957A58_exref != 0) {
    if ((*(int *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x11c) == 0) ||
       (*(char *)(*(int *)(*(int *)puVar3 + 0x44) + 0x75) == '\0')) {
      uVar6 = 0;
    }
    else {
      uVar6 = 1;
    }
    (**(code **)(*piVar1 + 0x6c))(piVar1,uVar6);
  }
  piVar1 = *(int **)(*(int *)PTR_DAT_0045a358 + 0x44);
  iVar4 = piVar1[0x35];
  *(undefined4 *)(iVar4 + 0x24) = 0;
  *(undefined4 *)(iVar4 + 0x1c) = 0;
  (**(code **)(*piVar1 + 100))
            (piVar1,(*(int *)(*(int *)(DAT_0045b2c0 + 0x44) + 0x80) -
                    *(int *)(*(int *)(*(int *)PTR_DAT_0045a0cc + 0x44) + 0x80)) -
                    *(int *)(*(int *)(*(int *)PTR_DAT_0045a098 + 0x44) + 0x80));
  (**(code **)(*piVar1 + 0x60))
            (piVar1,*(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a098 + 0x44) + 0x7c));
  (**(code **)(*piVar1 + 0x6c))(piVar1,0);
  iVar4 = *(int *)(*(int *)(*(int *)puVar3 + 0x44) + 0xd4);
  *(undefined1 *)(iVar4 + 0x29) = 3;
  *(undefined1 *)(iVar4 + 0x28) = 3;
  *(undefined4 *)(iVar4 + 0x20) = 0;
  *(undefined4 *)(iVar4 + 0x18) = 0;
  *(undefined1 *)(*(int *)puVar3 + 0x6e) = 3;
  *(undefined1 *)(*(int *)puVar3 + 0x6c) = 3;
  Classes_Rect_9923E97E
            (0,0,*(undefined4 *)(*(int *)(*(int *)puVar2 + 0x44) + 0x7c),auStack_20,
             *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a0cc + 0x44) + 0x80));
  puVar7 = auStack_20;
  puVar8 = (undefined4 *)(*(int *)puVar3 + 0x4c);
  for (iVar4 = 4; iVar4 != 0; iVar4 = iVar4 + -1) {
    *puVar8 = *puVar7;
    puVar7 = puVar7 + (uint)bVar9 * -2 + 1;
    puVar8 = puVar8 + (uint)bVar9 * -2 + 1;
  }
  *(undefined1 *)(*(int *)puVar3 + 0x6f) = 3;
  *(undefined1 *)(*(int *)puVar3 + 0x6d) = 3;
  Classes_Rect_9923E97E
            (0,0,*(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a098 + 0x44) + 0x7c),auStack_20,
             *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a0cc + 0x44) + 0x80));
  puVar7 = auStack_20;
  puVar8 = (undefined4 *)(*(int *)puVar3 + 0x5c);
  for (iVar4 = 4; iVar4 != 0; iVar4 = iVar4 + -1) {
    *puVar8 = *puVar7;
    puVar7 = puVar7 + (uint)bVar9 * -2 + 1;
    puVar8 = puVar8 + (uint)bVar9 * -2 + 1;
  }
  if (*(int *)AoWE_AoWHSMap_6B957A58_exref != 0) {
    if (*(int *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x11c) == 0) {
      *(undefined4 *)(*(int *)(*(int *)(*(int *)puVar3 + 0x44) + 0xd4) + 0x1c) =
           *(undefined4 *)(*(int *)(*(int *)puVar2 + 0x44) + 0x7c);
    }
    else {
      *(undefined4 *)(*(int *)(*(int *)(*(int *)puVar3 + 0x44) + 0xd4) + 0x1c) =
           *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a098 + 0x44) + 0x7c);
    }
  }
  *(undefined4 *)(*(int *)(*(int *)(*(int *)puVar3 + 0x44) + 0xd4) + 0x24) =
       *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a0cc + 0x44) + 0x80);
  if (*(char *)(*(int *)(*(int *)PTR_DAT_0045a490 + 0x44) + 0x75) != '\0') {
    FUN_00456aac(param_1,*(int *)(*(int *)PTR_DAT_0045a490 + 0x44));
  }
  *(undefined1 *)(*(int *)(param_1 + 0x44) + 0x2d) = 0;
  return;
}

