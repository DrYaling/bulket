use engine_api::{ObjectBase, IPtr};

use super::unit::IUnit;

/// world object with no motion
pub struct WorldObject{
    object: ObjectBase,
}
impl WorldObject{
    pub fn create(object: engine_api::ObjectBase) -> Self{
        Self { object }
    }
}
impl IUnit for WorldObject{
    #[inline]
    fn get_state(&self) -> &engine_api::FUnitState {
        &self.object.state
    }

    fn get_transform(&self) -> engine_api::Transform {
        self.object.GetTransform()
    }
    
    fn is_me(&self, actor: engine_api::AActor) -> bool {
        self.object.inner() == actor.inner()
    }
}