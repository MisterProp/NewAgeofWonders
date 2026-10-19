// /aowInt.dpl : AOWWinManager.TAOWWinManager.SetCursor@23EDC2EF

void AOWWinManager_TAOWWinManager_SetCursor_23EDC2EF(int param_1,short param_2)

{
                    /* 0x60cc  105  AOWWinManager.TAOWWinManager.SetCursor@23EDC2EF */
  if ((param_2 != *(short *)(param_1 + 0x30)) &&
     (*(short *)(param_1 + 0x30) = param_2, *(short *)(param_1 + 0x12a) != 0)) {
    (**(code **)(param_1 + 0x128))(*(undefined4 *)(param_1 + 300),param_1);
  }
  return;
}

