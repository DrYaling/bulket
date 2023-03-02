use super::unreal::*;

static mut BINDINGS: Option<UnrealBindings> = None;
#[inline]
pub fn set_bindings(bindings: UnrealBindings){
    unsafe{
        BINDINGS = Some(bindings);
    }
}
#[inline]
pub fn bindings() -> &'static UnrealBindings{
    unsafe{
        BINDINGS.as_ref().expect("engine was not initialized yet")
    }
}