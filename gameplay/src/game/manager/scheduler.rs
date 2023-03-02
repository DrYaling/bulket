//! time manager

use std::collections::HashMap;

use once_cell::sync::Lazy;
use shared::boxed::MutableBox;
type SchedulerHandler = Box<dyn Fn()>;
struct Scheduler{
    interval: f32,
    callback_times: u32,
    tick: f32,
    times: u32,
    handler: SchedulerHandler,
}
#[derive(Default)]
struct SchedulerManagerInternal{
    schedulers: HashMap<u32, Scheduler>,
    scheduler_id: u32,
    time_now: f32,
}
static SCHEDULERS: Lazy<MutableBox<SchedulerManagerInternal>> = Lazy::new(|| MutableBox::new(Default::default()));
pub struct SchedulerManager;
impl SchedulerManager{
    ///this was unsafe for unreal objects, life time can not be confirmed
    /// 
    /// use weak ptr instead if unreal objects is in scheduler
    pub fn add(handler: SchedulerHandler, times: u32, interval: f32){
        let mut schedulers = SCHEDULERS.lock_mut().unwrap();
        let scheduler = Scheduler{
            callback_times: 0,
            times,
            handler,
            interval,
            tick: 0f32,
        };
        let id = schedulers.scheduler_id;
        schedulers.scheduler_id = schedulers.scheduler_id.overflowing_add(1).0;
        schedulers.schedulers.insert(id, scheduler);
    }
    pub fn update(delta: f32){
        let mut schedulers = SCHEDULERS.lock_mut().unwrap();        
        let schedulers = &mut schedulers.data;
        schedulers.time_now += delta;
        let mut removed = vec![];
        for (id, scheduler) in &mut schedulers.schedulers {
            scheduler.tick += delta;
            if scheduler.tick > scheduler.interval{
                (scheduler.handler)();
                scheduler.callback_times += 1;
                scheduler.tick = 0.0;
            }
            if scheduler.callback_times >= scheduler.times{
                removed.push(*id);
            }
        }
        for id in removed {
            schedulers.schedulers.remove(&id);
        }
    }
    pub fn clear(){
        SCHEDULERS.lock_mut().unwrap().schedulers.clear();
    }
}
