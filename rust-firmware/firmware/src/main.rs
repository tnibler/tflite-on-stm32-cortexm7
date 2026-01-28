#![no_std]
#![no_main]
#![feature(c_variadic)]

use cortex_m as _;
use defmt::info;
use defmt_rtt as _;
use embassy_executor::Spawner;
use embassy_stm32::time::Hertz;
use panic_probe as _;

#[unsafe(no_mangle)]
unsafe extern "C" fn rust_ticks_per_second() -> u32 {
    embassy_time::TICK_HZ as u32
}

#[unsafe(no_mangle)]
unsafe extern "C" fn rust_current_time_ticks() -> u32 {
    embassy_time::Instant::now().as_ticks() as u32
}

#[unsafe(no_mangle)]
unsafe extern "C" fn DebugLog(
    str: *const core::ffi::c_char,
    args: core::ffi::VaList,
) -> core::ffi::c_int {
    use printf_compat::output;
    let display = unsafe { output::display(str, args) };
    // This adds a lot of newlines, but defmt can only do println.
    defmt::println!("{}", defmt::Display2Format(&display));
    display.bytes_written()
}

unsafe extern "C" {
    fn SayHello();
    fn RunModelFromRust(n: i32) -> i32;
    fn ProfileModelFromRust() -> i32;
}

#[embassy_executor::main]
async fn main(_spawner: Spawner) {
    let mut config = embassy_stm32::Config::default();
    {
        use embassy_stm32::rcc::*;
        config.rcc.hse = Some(Hse {
            freq: Hertz(24_000_000),
            mode: HseMode::Oscillator,
        });
        config.rcc.pll1 = Some(Pll {
            source: PllSource::HSE,
            prediv: PllPreDiv::DIV3,
            mul: PllMul::MUL150,
            divp: Some(PllDiv::DIV2),
            divq: None,
            divr: None,
            divs: None,
            divt: None,
        });
        config.rcc.sys = Sysclk::PLL1_P; // 600 Mhz
        config.rcc.ahb_pre = AHBPrescaler::DIV2; // 300 Mhz
        config.rcc.apb1_pre = APBPrescaler::DIV2; // 150 Mhz
        config.rcc.apb2_pre = APBPrescaler::DIV2; // 150 Mhz
        config.rcc.apb4_pre = APBPrescaler::DIV2; // 150 Mhz
        config.rcc.apb5_pre = APBPrescaler::DIV2; // 150 Mhz
        config.rcc.voltage_scale = VoltageScale::HIGH;
        config.rcc.mux.usbphycsel = mux::Usbphycsel::HSE;
        config.rcc.timer_prescaler = TimerPrescaler::DefaultX2;
    }
    let _p = embassy_stm32::init(config);
    unsafe {
        SayHello();
    }

    let r = unsafe { RunModelFromRust(1) };
    info!("Ran model: return status {}", r);

    unsafe { ProfileModelFromRust() };

    let bench = || {
        let iters = 100;
        let start_t = embassy_time::Instant::now();
        let r = unsafe { RunModelFromRust(iters) };
        assert_eq!(r, 0);
        info!(
            "{} iters: avg {}ms/iter",
            iters,
            start_t.elapsed().as_millis() as f32 / iters as f32
        );
    };

    bench();
    info!("Enabling instruction cache");
    let mut cor = cortex_m::Peripherals::take().unwrap();
    // Instruction cache, safe to enable
    cor.SCB.invalidate_icache();
    cor.SCB.enable_icache();
    for _ in 0..5 {
        bench();
    }

    // Data cache, breaks code using DMA if not using MPU or manually flushing before/after
    // transfers
    info!("Enabling data cache");
    cor.SCB.enable_dcache(&mut cor.CPUID);

    info!("Clearing data cache after every model run");
    for _ in 0..5 {
        bench();
        cor.SCB.clean_dcache(&mut cor.CPUID);
    }

    info!("Keeping data cache");
    loop {
        bench();
    }
}
