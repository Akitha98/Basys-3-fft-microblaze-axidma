#include <xparameters.h>
#include <xil_types.h>
#include <xsysmon.h>
#include <sleep.h>
#include <xil_printf.h>
#include "platform.h"
#include "xio.h"
#include "xaxidma.h"
#include "complex.h"

XSysMon xadc;
XSysMon_Config *xadc_config;
XAxiDma my_dma;
XAxiDma_Config *my_dma_config;
int init_addr = XPAR_BRAM_0_BASEADDR;
int init_addr1 = XPAR_BRAM_1_BASEADDR;
int status;

int main() {
    init_platform();
    u32 aux;

    xadc_config = XSysMon_LookupConfig(XPAR_SYSMON_0_DEVICE_ID);
    XSysMon_CfgInitialize(&xadc, xadc_config, xadc_config->BaseAddress);

    my_dma_config = XAxiDma_LookupConfigBaseAddr(XPAR_AXI_DMA_0_BASEADDR);
    XAxiDma_CfgInitialize(&my_dma, my_dma_config);
    xil_printf("hello world");
    // Collect data from XADC and store it in Block RAM
    for (int i = 0; i < 8; i++) {
        aux = XSysMon_GetAdcData(&xadc, 22);
        XIo_Out32(init_addr, aux);
        init_addr += 4;
        //usleep(200000);
    }

    // DMA to IP transfer configuration
    //XAxiDma_SimpleTransfer(&my_dma, XPAR_BRAM_0_BASEADDR, sizeof(u32) * 1024, XAXIDMA_DMA_TO_DEVICE);

   // usleep(2000000);
    // IP to DMA transfer configuration
    //XAxiDma_SimpleTransfer(&my_dma, XPAR_BRAM_1_BASEADDR, sizeof(u32) * 1024, XAXIDMA_DEVICE_TO_DMA);
    XAxiDma_SimpleTransfer(&my_dma, XPAR_BRAM_1_BASEADDR, sizeof(u32) * 8, XAXIDMA_DMA_TO_DEVICE);
    XAxiDma_SimpleTransfer(&my_dma, XPAR_BRAM_0_BASEADDR, sizeof(u32) * 8, XAXIDMA_DEVICE_TO_DMA);
    usleep(2000000);
    // Check DMA transfer status
    status = XAxiDma_ReadReg(XPAR_AXI_DMA_0_BASEADDR, 0x04) & XAXIDMA_HALTED_MASK;
    while (status != 1) {
        status = XAxiDma_ReadReg(XPAR_AXI_DMA_0_BASEADDR, 0x04) & XAXIDMA_HALTED_MASK;
    }

    status = XAxiDma_ReadReg(XPAR_AXI_DMA_0_BASEADDR, 0x34) & XAXIDMA_HALTED_MASK;
    while (status != 1) {
        status = XAxiDma_ReadReg(XPAR_AXI_DMA_0_BASEADDR, 0x34) & XAXIDMA_HALTED_MASK;
    }
    usleep(2000000);
    for(int i = 0; i < 8; i=i+4)
      {
        // Read values from Block RAM1
         u32 data = Xil_In32(XPAR_BRAM_0_BASEADDR+i);
         xil_printf("%d\r\n", data); // Print data to console (optional)

      }
    for(int i = 0; i < 8; i=i+4)
         {
           // Read values from Block RAM1
            u32 data = Xil_In32(XPAR_BRAM_1_BASEADDR+i);
            xil_printf("%d\r\n", data); // Print data to console (optional)

         }

    cleanup_platform();
    return 0;
}
