#include "sl_iostream.h"
#include "sl_iostream_uart.h"
#include "sl_iostream_eusart.h"
#include "sl_common.h"
#include "dmadrv.h"
#include "sl_device_peripheral.h"
#include "sl_clock_manager.h"


#include "sl_clock_manager_tree_config.h"
 


#include "em_eusart.h"
#include "em_gpio.h"
 

 
// Include instance config 
 #include "sl_iostream_eusart_inst0_config.h"

// MACROs for generating name and IRQ handler function  
#if defined(EUART_COUNT) && (EUART_COUNT > 0) 
#define SL_IOSTREAM_EUSART_TX_IRQ_NUMBER(periph_nbr)     SL_CONCAT_PASTER_3(EUART, periph_nbr, _TX_IRQn)   
#define SL_IOSTREAM_EUSART_RX_IRQ_NUMBER(periph_nbr)     SL_CONCAT_PASTER_3(EUART, periph_nbr, _RX_IRQn)   
#define SL_IOSTREAM_EUSART_TX_IRQ_HANDLER(periph_nbr)    SL_CONCAT_PASTER_3(EUART, periph_nbr, _TX_IRQHandler)   
#define SL_IOSTREAM_EUSART_RX_IRQ_HANDLER(periph_nbr)    SL_CONCAT_PASTER_3(EUART, periph_nbr, _RX_IRQHandler)   
#define SL_IOSTREAM_EUSART_RX_DMA_SIGNAL(periph_nbr)     SL_CONCAT_PASTER_3(dmadrvPeripheralSignal_EUART, periph_nbr, _RXDATAV)
#define SL_IOSTREAM_EUSART_CLOCK_REF(periph_nbr)         SL_CONCAT_PASTER_2(SL_BUS_CLOCK_EUART, periph_nbr)  
#define SL_IOSTREAM_EUSART_PERIPHERAL(periph_nbr)        SL_CONCAT_PASTER_2(SL_PERIPHERAL_EUART, periph_nbr) 
#else
#define SL_IOSTREAM_EUSART_TX_IRQ_NUMBER(periph_nbr)     SL_CONCAT_PASTER_3(EUSART, periph_nbr, _TX_IRQn)   
#define SL_IOSTREAM_EUSART_RX_IRQ_NUMBER(periph_nbr)     SL_CONCAT_PASTER_3(EUSART, periph_nbr, _RX_IRQn)   
#define SL_IOSTREAM_EUSART_TX_IRQ_HANDLER(periph_nbr)    SL_CONCAT_PASTER_3(EUSART, periph_nbr, _TX_IRQHandler)   
#define SL_IOSTREAM_EUSART_RX_IRQ_HANDLER(periph_nbr)    SL_CONCAT_PASTER_3(EUSART, periph_nbr, _RX_IRQHandler)   
#define SL_IOSTREAM_EUSART_RX_DMA_SIGNAL(periph_nbr)     SL_CONCAT_PASTER_3(dmadrvPeripheralSignal_EUSART, periph_nbr, _RXDATAV)  
#define SL_IOSTREAM_EUSART_CLOCK_REF(periph_nbr)         SL_CONCAT_PASTER_2(SL_BUS_CLOCK_EUSART, periph_nbr)  
#define SL_IOSTREAM_EUSART_PERIPHERAL(periph_nbr)        SL_CONCAT_PASTER_2(SL_PERIPHERAL_EUSART, periph_nbr) 
#endif



#define SL_IOSTREAM_EUSART_HWFC_NONE                     eusartHwFlowControlNone
#define SL_IOSTREAM_EUSART_HWFC_CTS                      eusartHwFlowControlCts
#define SL_IOSTREAM_EUSART_HWFC_RTS                      eusartHwFlowControlRts
#define SL_IOSTREAM_EUSART_HWFC_CTS_RTS                  eusartHwFlowControlCtsAndRts
 


#if defined(EUART_COUNT) && (EUART_COUNT > 0)
#define SL_IOSTREAM_EUSART_CLOCK_SOURCE(periph_nbr)      SL_CLOCK_MANAGER_EUART0CLK_SOURCE
#define SL_IOSTREAM_EUSART_HF_CLOCK_SOURCE               CMU_EUART0CLKCTRL_CLKSEL_EM01GRPACLK
#define SL_IOSTREAM_EUSART_DISABLED_CLOCK_SOURCE         CMU_EUART0CLKCTRL_CLKSEL_DISABLED
#else
#define SL_IOSTREAM_EUSART_CLOCK_SOURCE(periph_nbr)      SL_CLOCK_MANAGER_EUSART0CLK_SOURCE
#if defined(CMU_EUSART0CLKCTRL_CLKSEL_EM01GRPCCLK)
#define SL_IOSTREAM_EUSART_HF_CLOCK_SOURCE               CMU_EUSART0CLKCTRL_CLKSEL_EM01GRPCCLK
#elif defined(CMU_EUSART0CLKCTRL_CLKSEL_EM01GRPACLK)
#define SL_IOSTREAM_EUSART_HF_CLOCK_SOURCE               CMU_EUSART0CLKCTRL_CLKSEL_EM01GRPACLK
#endif
#define SL_IOSTREAM_EUSART_DISABLED_CLOCK_SOURCE         CMU_EUSART0CLKCTRL_CLKSEL_DISABLED
#endif

// Check clock configuration
 
#if (SL_IOSTREAM_EUSART_CLOCK_SOURCE(SL_IOSTREAM_EUSART_INST0_PERIPHERAL_NO) == \
    SL_IOSTREAM_EUSART_DISABLED_CLOCK_SOURCE)
  #error Peripheral clock is disabled for INST0. Modify sl_clock_manager_tree_config.h \
  to enable the peripheral clock
#else
#define _SL_IOSTREAM_EUSART_INST0_ENABLE_HIGH_FREQUENCY \
          SL_IOSTREAM_EUSART_CLOCK_SOURCE(SL_IOSTREAM_EUSART_INST0_PERIPHERAL_NO) == \
          SL_IOSTREAM_EUSART_HF_CLOCK_SOURCE

#if defined(SL_IOSTREAM_EUSART_INST0_ENABLE_HIGH_FREQUENCY) && \
  (_SL_IOSTREAM_EUSART_INST0_ENABLE_HIGH_FREQUENCY != \
  SL_IOSTREAM_EUSART_INST0_ENABLE_HIGH_FREQUENCY)
#if SL_IOSTREAM_EUSART_INST0_ENABLE_HIGH_FREQUENCY
#warning Configuration mismatch for IOStream EUSART INST0. \
 IOStream was configured in high-frequency, but peripheral uses a low-frequency \
 oscillator in sl_clock_manager_tree_config.h.
#else 
#warning Configuration mismatch for IOStream EUSART INST0. \
 IOStream was configured in low-frequency, but peripheral uses a high-frequency \
 oscillator in sl_clock_manager_tree_config.h.
#endif // SL_IOSTREAM_EUSART_INST0_ENABLE_HIGH_FREQUENCY
#endif // Config mismatch
#endif // SL_IOSTREAM_EUSART_DISABLED_CLOCK_SOURCE


 


// EM Events
#define SLEEP_EM_EVENT_MASK      (SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM0)
static void events_handler(sl_power_manager_em_t from,
                           sl_power_manager_em_t to);
static sl_power_manager_em_transition_event_info_t events_info =
{
  .event_mask = SLEEP_EM_EVENT_MASK,
  .on_event = events_handler,
};
static sl_power_manager_em_transition_event_handle_t events_handle;
 


sl_status_t sl_iostream_eusart_init_inst0(void);


// Instance(s) handle and context variable 
static sl_iostream_uart_t sl_iostream_inst0;
sl_iostream_t *sl_iostream_inst0_handle = &sl_iostream_inst0.stream;
sl_iostream_uart_t *sl_iostream_uart_inst0_handle = &sl_iostream_inst0;
static sl_iostream_eusart_context_t  context_inst0;
static uint8_t  rx_buffer_inst0[SL_IOSTREAM_EUSART_INST0_RX_BUFFER_SIZE];
sl_iostream_instance_info_t sl_iostream_instance_inst0_info = {
  .handle = &sl_iostream_inst0.stream,
  .name = "inst0",
  .type = SL_IOSTREAM_TYPE_UART,
  .periph_id = SL_IOSTREAM_EUSART_INST0_PERIPHERAL_NO,
  .init = sl_iostream_eusart_init_inst0,
};



sl_status_t sl_iostream_eusart_init_inst0(void)
{
  sl_status_t status;

  sl_iostream_eusart_config_t config_inst0 = { 
    .eusart = SL_IOSTREAM_EUSART_PERIPHERAL(SL_IOSTREAM_EUSART_INST0_PERIPHERAL_NO),
    .eusart_nbr = SL_IOSTREAM_EUSART_INST0_PERIPHERAL_NO,
    .bus_clock = SL_IOSTREAM_EUSART_CLOCK_REF(SL_IOSTREAM_EUSART_INST0_PERIPHERAL_NO),
    .baudrate = SL_IOSTREAM_EUSART_INST0_BAUDRATE,
    .parity = SL_IOSTREAM_EUSART_INST0_PARITY,
    .flow_control = SL_IOSTREAM_EUSART_INST0_FLOW_CONTROL_TYPE,
    .stop_bits = SL_IOSTREAM_EUSART_INST0_STOP_BITS,
    .enable_high_frequency = _SL_IOSTREAM_EUSART_INST0_ENABLE_HIGH_FREQUENCY,
#if defined(EUSART_COUNT) && (EUSART_COUNT > 1)
    .port_index = SL_IOSTREAM_EUSART_INST0_PERIPHERAL_NO,
#endif
    .tx_port = SL_IOSTREAM_EUSART_INST0_TX_PORT,
    .tx_pin = SL_IOSTREAM_EUSART_INST0_TX_PIN,
    .rx_port = SL_IOSTREAM_EUSART_INST0_RX_PORT,
    .rx_pin = SL_IOSTREAM_EUSART_INST0_RX_PIN,
#if defined(SL_IOSTREAM_EUSART_INST0_CTS_PORT)
    .cts_port = SL_IOSTREAM_EUSART_INST0_CTS_PORT,
    .cts_pin = SL_IOSTREAM_EUSART_INST0_CTS_PIN,
#endif
#if defined(SL_IOSTREAM_EUSART_INST0_RTS_PORT)
    .rts_port = SL_IOSTREAM_EUSART_INST0_RTS_PORT,
    .rts_pin = SL_IOSTREAM_EUSART_INST0_RTS_PIN,
#endif
  };

  sl_iostream_dma_config_t dma_config_inst0 = {.src = (uint8_t *)&SL_IOSTREAM_EUSART_INST0_PERIPHERAL->RXDATA,
                                                        .peripheral_signal = SL_IOSTREAM_EUSART_RX_DMA_SIGNAL(SL_IOSTREAM_EUSART_INST0_PERIPHERAL_NO)};
  sl_iostream_uart_config_t uart_config_inst0 = {
    .dma_cfg = dma_config_inst0,
    .rx_buffer = rx_buffer_inst0,
    .rx_buffer_length = SL_IOSTREAM_EUSART_INST0_RX_BUFFER_SIZE,
    .tx_irq_number = SL_IOSTREAM_EUSART_TX_IRQ_NUMBER(SL_IOSTREAM_EUSART_INST0_PERIPHERAL_NO),
    .rx_irq_number = SL_IOSTREAM_EUSART_RX_IRQ_NUMBER(SL_IOSTREAM_EUSART_INST0_PERIPHERAL_NO),
    .lf_to_crlf = SL_IOSTREAM_EUSART_INST0_CONVERT_BY_DEFAULT_LF_TO_CRLF,
    .rx_when_sleeping = SL_IOSTREAM_EUSART_INST0_RESTRICT_ENERGY_MODE_TO_ALLOW_RECEPTION,
  };
  uart_config_inst0.sw_flow_control = SL_IOSTREAM_EUSART_INST0_FLOW_CONTROL_TYPE == uartFlowControlSoftware;
  // Instantiate eusart instance 
  status = sl_iostream_eusart_init(&sl_iostream_inst0,
                                  &uart_config_inst0,
                                  &config_inst0,
                                  &context_inst0);
  EFM_ASSERT(status == SL_STATUS_OK);

   

  return status;
}



void sl_iostream_eusart_init_instances(void)
{
  
  // Enable power manager notifications
  sl_power_manager_subscribe_em_transition_event(&events_handle, &events_info);
   
  // Instantiate eusart instance(s) 
  
  sl_iostream_eusart_init_inst0();
  
}


void SL_IOSTREAM_EUSART_TX_IRQ_HANDLER(SL_IOSTREAM_EUSART_INST0_PERIPHERAL_NO)(void)
{
  sl_iostream_eusart_irq_handler(&sl_iostream_inst0);
}

void SL_IOSTREAM_EUSART_RX_IRQ_HANDLER(SL_IOSTREAM_EUSART_INST0_PERIPHERAL_NO)(void)
{
  sl_iostream_eusart_irq_handler(&sl_iostream_inst0);
}


 
static void events_handler(sl_power_manager_em_t from,
                           sl_power_manager_em_t to)
{
  (void) from;
  if (to < SL_POWER_MANAGER_EM3){
    // Only prepare for wakeup from EM2 or less, since EUSART doesn't run in EM3
    
    if (sl_iostream_uart_inst0_handle->stream.context != NULL) {
      sl_iostream_uart_prepare_for_sleep(sl_iostream_uart_inst0_handle);
    }
    
  }
}

