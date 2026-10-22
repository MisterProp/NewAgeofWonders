
void FUN_0044e2bc(int param_1)

{
  code *pcVar1;
  undefined4 uVar2;
  int *piVar3;
  undefined2 uStack_a;
  
  pcVar1 = *(code **)(param_1 + 0x5c);
  uVar2 = *(undefined4 *)(param_1 + 0x60);
  piVar3 = *(int **)(param_1 + 100);
  *(undefined4 *)(param_1 + 0x5c) = 0;
  *(undefined4 *)(param_1 + 0x60) = 0;
  *(undefined4 *)(param_1 + 100) = 0;
  uStack_a = (short)((uint)pcVar1 >> 0x10);
  if (uStack_a != 0) {
    (*pcVar1)(uVar2,piVar3);
  }
  if (piVar3 != (int *)0x0) {
    (**(code **)(*piVar3 + 0x2c))();
  }
  return;
}

