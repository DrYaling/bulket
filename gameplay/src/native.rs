//! native plugin ffi apis

use std::ffi::c_void;

use engine_api::{UnrealBindings, RustBindings, ResultCode};
use shared::logger::{init_logger};

#[no_mangle]
pub unsafe extern "C" fn RegisterInternalBinders(
    bindings: UnrealBindings,
    rust_bindings: *mut RustBindings,
) -> ResultCode {
    std::panic::set_hook(Box::new(|panic_info| {
        let info = panic_info
            .payload()
            .downcast_ref::<&'static str>()
            .copied()
            .or(panic_info
                .payload()
                .downcast_ref::<String>()
                .map(String::as_str));

        if let Some(s) = info {
            let location = panic_info.location().map_or("".to_string(), |loc| {
                format!("{}, at line {}", loc.file(), loc.line())
            });
            error!("Panic: {} => {}", location, s);
        } else {
            error!("panic occurred");
        }
    }));

    let r = std::panic::catch_unwind(|| {
        engine_api::set_bindings(bindings);
        shared::logger::bind_logger(engine_api::bindings().log);
        init_logger(4).unwrap();
        super::callback::build_bindings()
    });
    match r {
        Ok(bindings) => {
            *rust_bindings = bindings;
            info!("bind rust native success");
            ResultCode::Success
        }
        Err(_) => ResultCode::Panic,
    }
}
/// not to lua

#[no_mangle]
pub extern "C" fn ReleaseVector(ptr: *mut u8, size: u32, type_size: u32, cap: u32) {
    if ptr.is_null(){
        return;
    }
    unsafe{
        let len = (size * type_size) as usize;
        // debug!("ReleaseVector size {}, type_size {}, cap {}", size, type_size, cap);
        Vec::from_raw_parts(ptr, len, cap as usize);
    }
}

///auto generate code, do not modify
/// 
/// struct pointer release api
/// 
/// not to lua

#[no_mangle]
pub extern "C" fn ReleaseRefPtr(ptr: *mut c_void) {
    if ptr.is_null(){
        return;
    }
    unsafe{
        drop(Box::from_raw(ptr));
    }
}
#[no_mangle]
unsafe extern "C" fn on_dll_destroy(){
    super::callback::destroy_game();
}
#[repr(C)]
pub struct NativeBuffer{
    pub buffer: *mut u8,
    pub len: i32,
    pub capacity: u32,
    pub result: i32,
}
impl NativeBuffer{
    /// not to lua
    pub fn new() -> Self{
        Self { buffer: std::ptr::null_mut(), len: -1, capacity: 0, result:  -1}
    }
}