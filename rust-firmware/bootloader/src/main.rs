#![no_main]
#![no_std]

use embassy_stm32::gpio::{Level, Speed};
use embassy_time::Timer;
use flash_lib::{self, MEMORY_MAPPED_FLASH_ADDRESS, OpiFlashMemory, SpiFlashMemory};

#[cfg(feature = "defmt")]
use defmt::*;
#[cfg(feature = "defmt")]
use defmt_rtt as _;

use panic_probe as _;

#[cortex_m_rt::entry]
fn main() -> ! {
    let r = flash_lib::init();
    let mut cor = cortex_m::Peripherals::take().unwrap();

    let mut flash = SpiFlashMemory::new(r.flash_memory);

    let mut flash = flash.into_octo();
    flash.enable_mm();

    unsafe {
        // Set's the vector table offset register to the start of the flash memory.
        cor.SCB.vtor.write(MEMORY_MAPPED_FLASH_ADDRESS);
        // Bootload the flash memory by jumping to the start of the flash memory.
        cortex_m::asm::bootload(MEMORY_MAPPED_FLASH_ADDRESS as *const u32);
    }
}

#[unsafe(no_mangle)]
#[cfg_attr(target_os = "none", unsafe(link_section = ".HardFault.user"))]
unsafe extern "C" fn HardFault() {
    for _ in 0..10000 {
        cortex_m::asm::nop();
    }
    cortex_m::asm::udf();
    // cortex_m::peripheral::SCB::sys_reset();
}
