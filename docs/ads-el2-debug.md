# Debugging at EL2 (TF-A BL33 payload)

To debug binaries with Arm Development Studio, the FVP (or SoC) needs to be loaded with a looping binary. This binary is known as 'branch to self' or `bts.bin`, and has been included in the root directory. Arm Development Studio will then load a binary at the paused location, performing any required address translation.

At EL2, the `bts.bin` needs to be packaged into a TrustedFirmware-A BL33 payload. This can be done as follows:

```
make -C "bsp/arm-tf" PLAT=morello TARGET_PLATFORM=fvp clean

MBEDTLS_DIR="<project>/bsp/deps/mbedtls" \
CROSS_COMPILE="<toolchain>/bin/llvm-" \
LD_LIBRARY_PATH="<project>/output/fvp/intermediates/host_openssl/install/lib" \
make -C "bsp/arm-tf" \
CC="<toolchain>/bin/clang" \
LD="<toolchain>/bin/ld.lld" \
PLAT=morello ARCH=aarch64 TARGET_PLATFORM=fvp ENABLE_MORELLO_CAP=1 \
E=0 TRUSTED_BOARD_BOOT=1 GENERATE_COT=1 ARM_ROTPK_LOCATION="devel_rsa" \
ROT_KEY="plat/arm/board/common/rotpk/arm_rotprivk_rsa.pem" \
BL33="<terios>/debug-loop/bts.bin" \
OPENSSL_DIR="<project>/output/fvp/intermediates/host_openssl/install" \
all fip
```

Again, replace `<project>` and `<terios>` with the correct paths. `<toolchain>` is the path to the `llvm-project-releases` directory.

The FVP can now be started using the following command:

```
<project>/model/FVP_Morello/models/Linux64_GCC-6.4/FVP_Morello \
    --data Morello_Top.css.scp.armcortexm7ct=<project>/bsp/rom-binaries/scp_romfw.bin@0x0 \
    --data Morello_Top.css.mcp.armcortexm7ct=<project>/bsp/rom-binaries/mcp_romfw.bin@0x0 \
    -C Morello_Top.soc.scp_qspi_loader.fname=<project>/output/fvp/firmware/scp_fw.bin \
    -C Morello_Top.soc.mcp_qspi_loader.fname=<project>/output/fvp/firmware/mcp_fw.bin \
    -C css.scp.armcortexm7ct.INITVTOR=0x0 \
    -C css.mcp.armcortexm7ct.INITVTOR=0x0 \
    -C css.trustedBootROMloader.fname=<project>/bsp/rom-binaries/bl1.bin \
    -C board.ap_qspi_loader.fname=<project>/bsp/arm-tf/build/morello/release/fip.bin \
    -C css.pl011_uart_ap.unbuffered_output=1 --run --cadi-server
```
