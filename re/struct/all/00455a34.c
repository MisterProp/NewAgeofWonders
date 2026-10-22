
void FUN_00455a34(int param_1)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  
  if (*(char *)(param_1 + 0x4c) == '\0') {
    *(undefined1 *)(param_1 + 0x4c) = 1;
    *(undefined1 *)(param_1 + 0x58) =
         *(undefined1 *)(*(int *)(*(int *)PTR_DAT_0045a4fc + 0x44) + 0x75);
    *(undefined1 *)(param_1 + 0x59) =
         *(undefined1 *)(*(int *)(*(int *)PTR_DAT_0045a084 + 0x44) + 0x75);
    *(undefined1 *)(param_1 + 0x5a) =
         *(undefined1 *)(*(int *)(*(int *)PTR_DAT_0045a53c + 0x44) + 0x75);
    *(undefined1 *)(param_1 + 0x5d) =
         *(undefined1 *)(*(int *)(*(int *)PTR_DAT_0045a0b0 + 0x44) + 0x75);
    *(undefined1 *)(param_1 + 0x65) =
         *(undefined1 *)(*(int *)(*(int *)PTR_DAT_0045a394 + 0x44) + 0x75);
    *(undefined1 *)(param_1 + 0x5e) =
         *(undefined1 *)(*(int *)(*(int *)PTR_DAT_0045a1c0 + 0x44) + 0x75);
    *(undefined1 *)(param_1 + 0x5f) =
         *(undefined1 *)(*(int *)(*(int *)PTR_DAT_0045a5c4 + 0x44) + 0x75);
    *(undefined1 *)(param_1 + 0x60) =
         *(undefined1 *)(*(int *)(*(int *)PTR_DAT_0045a490 + 0x44) + 0x75);
    *(undefined1 *)(param_1 + 0x61) =
         *(undefined1 *)(*(int *)(*(int *)PTR_DAT_0045a5bc + 0x44) + 0x75);
    iVar4 = *(int *)(*(int *)(*(int *)(DAT_0045b2c0 + 0x44) + 0xd0) + 8);
    if (-1 < iVar4 + -1) {
      iVar3 = 0;
      do {
        iVar1 = Classes_TList_Get_23EDC2EF
                          (*(undefined4 *)(*(int *)(DAT_0045b2c0 + 0x44) + 0xd0),iVar3);
        if (iVar1 != *(int *)(*(int *)PTR_DAT_0045a0cc + 0x44)) {
          piVar2 = (int *)Classes_TList_Get_23EDC2EF
                                    (*(undefined4 *)(*(int *)(DAT_0045b2c0 + 0x44) + 0xd0),iVar3);
          (**(code **)(*piVar2 + 0x6c))(piVar2,0);
        }
        iVar3 = iVar3 + 1;
        iVar4 = iVar4 + -1;
      } while (iVar4 != 0);
    }
  }
  return;
}

