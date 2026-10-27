// TAOWHScrollBar.CheckMouseMove (/aowInt.dpl 0x5981fa5c)


void AoWHScroll_TAOWHScrollBar_CheckMouseMove_23EDC2EF
               (int param_1,undefined1 param_2,int param_3,int param_4)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
                    /* 0x1fa5c  512  AoWHScroll.TAOWHScrollBar.CheckMouseMove@23EDC2EF */
  AoWControl_TAOWControl_CheckMouseMove_23EDC2EF(param_1,param_2,param_3,param_4);
  *(undefined4 *)(param_1 + 0x78) = 0;
  if ((*(char *)(param_1 + 0x151) == '\x05') && (*(char *)(param_1 + 0x150) != '\0')) {
    iVar1 = ((param_3 - *(int *)(param_1 + 0x154)) * *(int *)(param_1 + 0x14c)) /
            *(int *)(param_1 + 0x130) + *(int *)(param_1 + 0x138);
    *(int *)(param_1 + 0x148) = iVar1;
    if (iVar1 < *(int *)(param_1 + 0x138)) {
      *(int *)(param_1 + 0x148) = *(int *)(param_1 + 0x138);
    }
    iVar1 = (*(int *)(param_1 + 0x13c) + 1) - *(int *)(param_1 + 0x158);
    if (iVar1 < *(int *)(param_1 + 0x148)) {
      *(int *)(param_1 + 0x148) = iVar1;
    }
    *(undefined1 *)(param_1 + 0xa4) = 0;
    if (*(short *)(param_1 + 0x6e) != 0) {
      (**(code **)(param_1 + 0x6c))(*(undefined4 *)(param_1 + 0x70),param_1);
    }
  }
  iVar1 = *(int *)(param_1 + 0x10c);
  iVar3 = *(int *)(iVar1 + 0x84) + *(int *)(param_1 + 0x84);
  iVar2 = iVar3 + -1;
  if ((((iVar2 < param_3) && (param_3 < iVar3 + *(int *)(param_1 + 0x7c))) &&
      (*(int *)(iVar1 + 0x88) + *(int *)(param_1 + 0x88) + -1 < param_4)) &&
     ((param_4 < *(int *)(iVar1 + 0x88) + *(int *)(param_1 + 0x88) + *(int *)(param_1 + 0x80) &&
      (*(undefined4 *)(param_1 + 0x78) = 0xffffffff, *(short *)(param_1 + 0x56) != 0)))) {
    (**(code **)(param_1 + 0x54))
              (*(undefined4 *)(param_1 + 0x58),param_1,CONCAT31((int3)((uint)iVar2 >> 8),param_2),
               param_4,param_3);
  }
  return;
}

