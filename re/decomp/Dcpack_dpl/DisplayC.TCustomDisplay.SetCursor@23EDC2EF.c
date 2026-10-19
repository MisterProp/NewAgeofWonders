// /Dcpack.dpl : DisplayC.TCustomDisplay.SetCursor@23EDC2EF

void DisplayC_TCustomDisplay_SetCursor_23EDC2EF(int param_1,undefined4 param_2)

{
                    /* 0x4b44  106  DisplayC.TCustomDisplay.SetCursor@23EDC2EF */
  Controls_TControl_SetCursor_23EDC2EF(param_1,param_2);
  if (*(int *)(param_1 + 0x118) != 0) {
    Controls_TControl_SetCursor_23EDC2EF(*(int *)(param_1 + 0x118),param_2);
  }
  return;
}

