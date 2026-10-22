
void FUN_004247b8(int param_1)

{
  int *piVar1;
  code *pcVar2;
  undefined4 uVar3;
  undefined2 uStack_a;
  
  piVar1 = *(int **)(param_1 + 0x29c);
  if (piVar1 != (int *)0x0) {
    pcVar2 = *(code **)(param_1 + 0x294);
    uVar3 = *(undefined4 *)(param_1 + 0x298);
    *(undefined4 *)(param_1 + 0x294) = 0;
    *(undefined4 *)(param_1 + 0x298) = 0;
    *(undefined4 *)(param_1 + 0x29c) = 0;
    uStack_a = (short)((uint)pcVar2 >> 0x10);
    if (uStack_a != 0) {
      (*pcVar2)(uVar3,piVar1);
    }
    if (piVar1 != (int *)0x0) {
      (**(code **)(*piVar1 + 0x2c))();
    }
  }
  return;
}

