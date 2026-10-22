
void Crops_TCrop_SetCropOwner_23EDC2EF(int param_1,undefined4 param_2)

{
                    /* 0xa5cbc  6763  Crops.TCrop.SetCropOwner@23EDC2EF */
  *(undefined4 *)(param_1 + 0x1c) = param_2;
  Crops_TCrop_UpdateSettings_23EDC2EF();
  return;
}

