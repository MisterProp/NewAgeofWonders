
void GFXE_GenerateAlphaBlendTable_E097D0B4(int param_1)

{
  char cVar1;
  undefined1 uVar2;
  int iVar3;
  int iVar4;
  
                    /* 0xa578  100  GFXE.GenerateAlphaBlendTable@E097D0B4 */
  cVar1 = System__IsClass_51F89FF7(param_1,&GFXE__TBlendTable5Bit_7FE8F72A);
  if (cVar1 == '\0') {
    SysUtils_Exception_Create_23EDC2EF
              (SysUtils__Exception_9880D0BF_exref,1,"BlendTable format not supported");
    System__RaiseExcept_51F89FF7();
  }
  else {
    iVar4 = 0;
    do {
      iVar3 = 0;
      do {
        uVar2 = System__ROUND_51F89FF7();
        *(undefined1 *)(param_1 + iVar4 * 0x20 + 0xc + iVar3) = uVar2;
        iVar3 = iVar3 + 1;
      } while (iVar3 != 0x20);
      iVar4 = iVar4 + 1;
    } while (iVar4 != 0x20);
  }
  return;
}


/* WARNING: Unable to track spacebase fully for stack */

byte * GFXE_BlendColorKey565_981312BC(void)

{
  ushort uVar1;
  ushort uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  byte *pbVar6;
  uint uVar7;
  uint *puVar8;
  uint *puVar9;
  bool bVar10;
  int in_stack_00000004;
  byte *in_stack_00000008;
  int in_stack_0000000c;
  uint in_stack_00000010;
  int in_stack_00000014;
  int in_stack_00000018;
  uint *in_stack_0000001c;
  uint *in_stack_00000020;
  uint *local_34;
  uint local_30;
  uint local_10;
  uint local_c;
  
                    /* 0xb3f0  77  GFXE.BlendColorKey565@981312BC */
  pbVar6 = (byte *)0x0;
  if (in_stack_0000000c != 0) {
    bVar10 = ((uint)in_stack_0000001c & 3) != 0;
    local_10 = in_stack_00000010;
    if (bVar10) {
      local_10 = in_stack_00000010 - 1;
    }
    local_c = local_10 >> 1;
    local_10 = local_10 & 1;
    uVar7 = (int)in_stack_00000008 << 0x10;
    uVar4 = (uint)in_stack_00000008 | uVar7;
    do {
      puVar8 = in_stack_0000001c;
      if (bVar10) {
        uVar2 = (ushort)*in_stack_00000020;
        if ((byte *)(uint)uVar2 == in_stack_00000008) {
          in_stack_00000020 = (uint *)((int)in_stack_00000020 + 2);
          puVar8 = (uint *)((int)in_stack_0000001c + 2);
        }
        else {
          uVar1 = (ushort)*in_stack_0000001c;
          puVar8 = (uint *)((int)in_stack_0000001c + 2);
          in_stack_00000020 = (uint *)((int)in_stack_00000020 + 2);
          *(ushort *)in_stack_0000001c =
               (ushort)*(byte *)(((uVar2 & 0x7c0) >> 1) + in_stack_00000004 + (uVar1 >> 6 & 0x1f)) *
               0x40 + (ushort)*(byte *)((uVar1 & 0x1f) + in_stack_00000004 + (uVar2 & 0x1f) * 0x20)
               + (ushort)*(byte *)((uVar2 >> 6 & 0x3e0) + in_stack_00000004 + (uint)(uVar1 >> 0xb))
                 * 0x800;
        }
      }
      if (local_c != 0) {
        local_30 = local_c;
        uVar5 = *in_stack_00000020;
LAB_5500b50c:
        do {
          while (uVar5 == uVar4) {
LAB_5500b4fa:
            in_stack_00000020 = in_stack_00000020 + 1;
            puVar8 = puVar8 + 1;
            local_30 = local_30 - 1;
            if (local_30 == 0) goto LAB_5500b739;
            uVar5 = *in_stack_00000020;
          }
          puVar9 = puVar8;
          if ((uVar5 & 0xffff0000) == uVar7) {
LAB_5500b64d:
            uVar2 = (ushort)*puVar9;
            puVar8 = puVar9 + 1;
            in_stack_00000020 = in_stack_00000020 + 1;
            *(ushort *)puVar9 =
                 (ushort)*(byte *)((uVar5 >> 1 & 0x3e0) + in_stack_00000004 + (uVar2 >> 6 & 0x1f)) *
                 0x40 + (ushort)*(byte *)((uVar2 & 0x1f) + in_stack_00000004 + (uVar5 & 0x1f) * 0x20
                                         ) +
                 (ushort)*(byte *)((uVar5 >> 6 & 0x3e0) + in_stack_00000004 + (uint)(uVar2 >> 0xb))
                 * 0x800;
            local_30 = local_30 - 1;
            if (local_30 == 0) break;
            uVar5 = *in_stack_00000020;
            goto LAB_5500b50c;
          }
          local_34 = in_stack_00000020;
          if ((byte *)(uVar5 & 0xffff) != in_stack_00000008) {
            do {
              uVar3 = *puVar9;
              puVar8 = puVar9 + 1;
              in_stack_00000020 = local_34 + 1;
              *puVar9 = (uint)*(byte *)((uVar3 >> 0xb & 0x1f) +
                                       (uVar5 >> 6 & 0x3e0) + in_stack_00000004) * 0x800 +
                        (uint)*(byte *)((uVar3 & 0x1f) + in_stack_00000004 + (uVar5 & 0x1f) * 0x20)
                        + (uint)*(byte *)((uVar5 >> 1 & 0x3e0) + in_stack_00000004 +
                                         (uVar3 >> 6 & 0x1f)) * 0x40 +
                        ((uint)*(byte *)((uVar5 >> 0x16 & 0x3e0) + in_stack_00000004 +
                                        (uVar3 >> 0x1b)) * 0x800 +
                        (uint)*(byte *)((uVar5 >> 0x11 & 0x3e0) + in_stack_00000004 +
                                       (uVar3 >> 0x16 & 0x1f)) * 0x40 +
                        (uint)*(byte *)((uVar3 >> 0x10 & 0x1f) + in_stack_00000004 +
                                       (uVar5 >> 0x10 & 0x1f) * 0x20)) * 0x10000;
              local_30 = local_30 - 1;
              if (local_30 == 0) goto LAB_5500b739;
              uVar5 = *in_stack_00000020;
              if ((uVar5 & 0xffff0000) == uVar7) {
                puVar9 = puVar8;
                if (uVar5 == uVar4) goto LAB_5500b4fa;
                goto LAB_5500b64d;
              }
              puVar9 = puVar8;
              local_34 = in_stack_00000020;
            } while ((byte *)(uVar5 & 0xffff) != in_stack_00000008);
            if (uVar5 == uVar4) goto LAB_5500b4fa;
          }
          uVar2 = *(ushort *)((int)puVar9 + 2);
          puVar8 = puVar9 + 1;
          in_stack_00000020 = in_stack_00000020 + 1;
          *(ushort *)((int)puVar9 + 2) =
               (ushort)*(byte *)((uVar5 >> 0x11 & 0x3e0) + in_stack_00000004 + (uVar2 >> 6 & 0x1f))
               * 0x40 + (ushort)*(byte *)((uVar2 & 0x1f) + in_stack_00000004 +
                                         (uVar5 >> 0x10 & 0x1f) * 0x20) +
               (ushort)*(byte *)((uVar5 >> 0x16 & 0x3e0) + in_stack_00000004 + (uint)(uVar2 >> 0xb))
               * 0x800;
          local_30 = local_30 - 1;
          if (local_30 == 0) break;
          uVar5 = *in_stack_00000020;
        } while( true );
      }
LAB_5500b739:
      pbVar6 = (byte *)0x0;
      in_stack_0000001c = puVar8;
      if (local_10 != 0) {
        uVar2 = (ushort)*in_stack_00000020;
        pbVar6 = (byte *)(uint)uVar2;
        if (pbVar6 == in_stack_00000008) {
          in_stack_00000020 = (uint *)((int)in_stack_00000020 + 2);
          in_stack_0000001c = (uint *)((int)puVar8 + 2);
        }
        else {
          uVar1 = (ushort)*puVar8;
          pbVar6 = (byte *)((((uint)pbVar6 & 0x7c0) >> 1) + in_stack_00000004 + (uVar1 >> 6 & 0x1f))
          ;
          in_stack_0000001c = (uint *)((int)puVar8 + 2);
          in_stack_00000020 = (uint *)((int)in_stack_00000020 + 2);
          *(ushort *)puVar8 =
               (ushort)*pbVar6 * 0x40 +
               (ushort)*(byte *)((uVar1 & 0x1f) + in_stack_00000004 + (uVar2 & 0x1f) * 0x20) +
               (ushort)*(byte *)((uVar2 >> 6 & 0x3e0) + in_stack_00000004 + (uint)(uVar1 >> 0xb)) *
               0x800;
        }
      }
      in_stack_00000020 =
           (uint *)((int)in_stack_00000020 + in_stack_00000018 + in_stack_00000010 * -2);
      in_stack_0000001c =
           (uint *)((int)in_stack_0000001c + in_stack_00000014 + in_stack_00000010 * -2);
      in_stack_0000000c = in_stack_0000000c + -1;
    } while (in_stack_0000000c != 0);
  }
  return pbVar6;
}


void GFXE_TBlendTable_Initialize_23EDC2EF(int param_1,undefined4 param_2,undefined4 param_3)

{
                    /* 0xa898  96  GFXE.TBlendTable.Initialize@23EDC2EF */
  *(undefined4 *)(param_1 + 8) = param_3;
  *(undefined4 *)(param_1 + 4) = param_2;
  return;
}

