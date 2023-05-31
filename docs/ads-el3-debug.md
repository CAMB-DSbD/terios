# Debugging at EL3

To debug binaries with Arm Development Studio, the FVP (or SoC) needs to be loaded with a looping binary. This binary is known as 'branch to self' or `bts.bin`, and has been included in the root directory. Arm Development Studio will then load a binary at the paused location, performing any required address translation.

The FVP can be run using the following command. Replace `<project>` and `<terios>` with the correct paths.

```
<project>/model/FVP_Morello/models/Linux64_GCC-6.4/FVP_Morello \
    --data Morello_Top.css.scp.armcortexm7ct=<project>/bsp/rom-binaries/scp_romfw.bin@0x0 \
    --data Morello_Top.css.mcp.armcortexm7ct=<project>/bsp/rom-binaries/mcp_romfw.bin@0x0 \
    -C Morello_Top.soc.scp_qspi_loader.fname=<project>/output/fvp/firmware/scp_fw.bin \
    -C Morello_Top.soc.mcp_qspi_loader.fname=<project>/output/fvp/firmware/mcp_fw.bin \
    -C css.scp.armcortexm7ct.INITVTOR=0x0 \
    -C css.mcp.armcortexm7ct.INITVTOR=0x0 \
    --data=<terios>/debug-loop/bts.bin@0x14000000 \
    -C soc.scc.boot_gpr_2=0x14000000 \
    -C soc.scc.boot_gpr_3=0 \
    -C css.cluster0.cpu0.semihosting-heap_base=0 \
    -C css.cluster0.cpu0.semihosting-heap_limit=0xff000000 \
    -C css.cluster0.cpu0.semihosting-stack_limit=0xff000000 \
    -C css.cluster0.cpu0.semihosting-stack_base=0xffff0000 \
    --run \
    --cadi-server
```

Arm Development Studio can now be used to load and debug a binary.
