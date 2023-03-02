
mod heal;
mod damage;
pub use heal::*;
pub use damage::*;
use shared::Vector;

//TODO
#[derive(Clone)]
pub enum SkillEffect{
    Heal(Vector<HeadEffect>),
    Damage(Vector<Damage>)
}