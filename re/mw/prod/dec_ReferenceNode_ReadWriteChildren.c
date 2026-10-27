
void Engine_TEReferenceNode_ReadWriteChildren_23EDC2EF(int param_1,undefined4 *param_2)

{
  int iVar1;
  byte bVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  
                    /* 0x19edc  205  Engine.TEReferenceNode.ReadWriteChildren@23EDC2EF */
  uVar3 = (**(code **)*param_2)();
  if ((uVar3 & 2) == 0) {
    Engine_TECustomNode_ReadWriteChildren_23EDC2EF(param_1,param_2);
    bVar2 = (**(code **)*param_2)();
    if (((bVar2 & 0x45) == 1) && (iVar4 = *(int *)(*(int *)(param_1 + 8) + 8), -1 < iVar4 + -1)) {
      iVar5 = 0;
      do {
        (**(code **)(**(int **)(*(int *)(*(int *)(param_1 + 8) + 4) + iVar5 * 4) + 0x2c))();
        iVar5 = iVar5 + 1;
        iVar4 = iVar4 + -1;
      } while (iVar4 != 0);
    }
  }
  else {
    iVar4 = *(int *)(*(int *)(param_1 + 8) + 8);
    if (-1 < iVar4 + -1) {
      iVar5 = 0;
      do {
        iVar1 = *(int *)(*(int *)(*(int *)(param_1 + 8) + 4) + iVar5 * 4);
        if (*(int *)(iVar1 + 4) == 0) {
          *(int *)(iVar1 + 4) = param_1;
        }
        iVar5 = iVar5 + 1;
        iVar4 = iVar4 + -1;
      } while (iVar4 != 0);
    }
    Engine_TECustomNode_ReadWriteChildren_23EDC2EF(param_1,param_2);
    iVar4 = *(int *)(*(int *)(param_1 + 8) + 8);
    if (-1 < iVar4 + -1) {
      iVar5 = 0;
      do {
        iVar1 = *(int *)(*(int *)(*(int *)(param_1 + 8) + 4) + iVar5 * 4);
        if (param_1 == *(int *)(iVar1 + 4)) {
          *(undefined4 *)(iVar1 + 4) = 0;
        }
        iVar5 = iVar5 + 1;
        iVar4 = iVar4 + -1;
      } while (iVar4 != 0);
    }
  }
  return;
}

