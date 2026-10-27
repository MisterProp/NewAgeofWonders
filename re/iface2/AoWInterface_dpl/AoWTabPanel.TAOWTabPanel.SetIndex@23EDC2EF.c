// /AoWInterface.dpl : AoWTabPanel.TAOWTabPanel.SetIndex@23EDC2EF

void AoWTabPanel_TAOWTabPanel_SetIndex_23EDC2EF(int param_1,int param_2)

{
                    /* 0x141c0  315  AoWTabPanel.TAOWTabPanel.SetIndex@23EDC2EF */
  if (param_2 != *(int *)(param_1 + 0x114)) {
    *(int *)(param_1 + 0x114) = param_2;
    *(undefined1 *)(param_1 + 0xd2) = 0;
  }
  return;
}

