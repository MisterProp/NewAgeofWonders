
void FUN_00438560(int param_1)

{
  code *pcVar1;
  undefined4 uVar2;
  int *piVar3;
  undefined2 uStack_e;
  
  (**(code **)(**(int **)(param_1 + 0xa4) + 0x30))();
  if (*(char *)(param_1 + 0xa0) != '\0') {
    *(undefined1 *)(param_1 + 0xa0) = 0;
    (**(code **)(**(int **)(*(int *)PTR_DAT_0045a53c + 0x44) + 0x68))
              (*(int **)(*(int *)PTR_DAT_0045a53c + 0x44),
               *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a084 + 0x44) + 0xa0));
    (**(code **)(**(int **)(*(int *)PTR_DAT_0045a53c + 0x44) + 0x6c))
              (*(int **)(*(int *)PTR_DAT_0045a53c + 0x44),0);
  }
  pcVar1 = *(code **)(param_1 + 0x94);
  uVar2 = *(undefined4 *)(param_1 + 0x98);
  piVar3 = *(int **)(param_1 + 0x9c);
  *(undefined4 *)(param_1 + 0x94) = 0;
  *(undefined4 *)(param_1 + 0x98) = 0;
  *(undefined4 *)(param_1 + 0x9c) = 0;
  uStack_e = (short)((uint)pcVar1 >> 0x10);
  if (uStack_e != 0) {
    (*pcVar1)(uVar2,piVar3);
  }
  if (piVar3 != (int *)0x0) {
    (**(code **)(*piVar3 + 0x2c))();
  }
  return;
}

