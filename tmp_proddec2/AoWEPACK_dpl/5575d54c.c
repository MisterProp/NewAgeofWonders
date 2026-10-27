// /AoWEPACK.dpl : 5575d54c

int AoWE_TProductionControl_CreateProductionTE_23EDC2EF(int *param_1)

{
  undefined1 uVar1;
  int iVar2;
  
                    /* 0x5d54c  2351  AoWE.TProductionControl.CreateProductionTE@23EDC2EF */
  iVar2 = NetworkE_TTokenEvent_Create_23EDC2EF(&AoWE__TProductionTE_16BAD1F0,1,0);
  (**(code **)(*param_1 + 0x78))(param_1,iVar2 + 0x10,iVar2 + 0x11,iVar2 + 0x12);
  uVar1 = (**(code **)(*param_1 + 0x7c))();
  *(undefined1 *)(iVar2 + 0x13) = uVar1;
  return iVar2;
}

