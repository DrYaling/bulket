//! input manager

use engine_api::{EGameInputType, ETriggerEvent, ResultCode};
use once_cell::sync::Lazy;
use shared::boxed::MutableBox;
///game input
#[derive(Debug, Clone, Copy)]
pub struct Input{
    pub input: EGameInputType, 
    pub trigger_event: ETriggerEvent, 
    pub elapsed: f32
}
impl Input{
    #[inline]
    pub fn new(
        input: EGameInputType, 
        trigger_event: ETriggerEvent, 
        elapsed: f32
    ) -> Self{
        Self { input, trigger_event, elapsed }
    }
}
struct InputInner{
    current: Option<Input>,
    #[allow(unused)]
    //cache at most 10 input    
    input_queue: [Input; 10],
}
static INPUT: Lazy<MutableBox<InputInner>> = Lazy::new(|| {
    MutableBox::new(InputInner{
        current: None, 
        input_queue: [Input::new(EGameInputType::None, ETriggerEvent::None, 0.0); 10]
    })
});
pub struct InputManager;
impl InputManager{
    pub fn on_game_input(
        input: EGameInputType, 
        trigger_event: ETriggerEvent, 
        elapsed: f32
    ) -> ResultCode{
        if let Some(mut input_manager) = INPUT.get_mut(Some(16)){
            input_manager.current = Some(Input::new(input, trigger_event, elapsed));
            ResultCode::Success
        }
        else{
            ResultCode::Success
        }
    }
    ///get current input
    pub fn get_input(consume: bool) -> Option<Input>{
        if consume{
            INPUT.lock_mut().unwrap().current.take()
        }
        else{
            INPUT.lock_ref()?.current.clone()
        }
    }
}