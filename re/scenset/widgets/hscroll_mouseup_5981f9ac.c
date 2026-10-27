// TAOWHScrollBar.CheckMouseUp (/aowInt.dpl 0x5981f9ac)


void AoWHScroll_TAOWHScrollBar_CheckMouseUp_23EDC2EF
               (int param_1,undefined1 param_2,undefined1 param_3,int param_4,int param_5)

{
  int iVar1;
  int iVar2;
  
                    /* 0x1f9ac  513  AoWHScroll.TAOWHScrollBar.CheckMouseUp@23EDC2EF */
  *(undefined4 *)(param_1 + 0x78) = 0;
  if ((*(char *)(param_1 + 0x151) == '\x01') || (*(char *)(param_1 + 0x151) == '\x04')) {
    *(undefined1 *)(param_1 + 0xa4) = 0;
  }
  *(undefined1 *)(param_1 + 0x150) = 0;
  *(undefined1 *)(param_1 + 0x151) = 0;
  iVar1 = *(int *)(param_1 + 0x10c);
  iVar2 = *(int *)(iVar1 + 0x84) + *(int *)(param_1 + 0x84);
  if ((((iVar2 + -1 < param_5) && (param_5 < iVar2 + *(int *)(param_1 + 0x7c))) &&
      (*(int *)(iVar1 + 0x88) + *(int *)(param_1 + 0x88) + -1 < param_4)) &&
     ((iVar2 = *(int *)(iVar1 + 0x88) + *(int *)(param_1 + 0x88) + *(int *)(param_1 + 0x80),
      param_4 < iVar2 &&
      (*(undefined4 *)(param_1 + 0x78) = 0xffffffff, *(short *)(param_1 + 0x4e) != 0)))) {
    (**(code **)(param_1 + 0x4c))
              (*(undefined4 *)(param_1 + 0x50),param_1,CONCAT31((int3)((uint)iVar1 >> 8),param_2),
               param_4,param_5,CONCAT31((int3)((uint)iVar2 >> 8),param_3));
  }
  return;
}

