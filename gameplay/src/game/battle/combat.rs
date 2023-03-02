use shared::boxed::MutexArc;

use crate::game::entity::{Character, Creature, IUnit};

/*
combat stat
 */
#[derive(Default)]
pub struct Combat{
    pub(crate) characters: Vec<MutexArc<Character>>,
    pub(crate) monsters: Vec<MutexArc<Creature>>,    
    combat_id: u32,
    valid: bool,
}
impl Combat{
    pub fn new(
        combat_id: u32,
        characters: Vec<MutexArc<Character>>,
        monsters: Vec<MutexArc<Creature>>,    
    ) -> Self{
        characters.iter().for_each(|c| c.mut_checked().leave_combat());
        monsters.iter().for_each(|c| c.mut_checked().leave_combat());
        Self { characters, monsters, combat_id, valid: true}
    }
    pub fn destroy(&mut self){
        self.characters.iter().for_each(|c| c.mut_checked().leave_combat());
        self.monsters.iter().for_each(|c| c.mut_checked().leave_combat());
        self.valid = false;
        info!("combat {} stopped", self.combat_id);
    }
    pub fn update(&mut self, delta: f32){
        if !self.valid{
            return;
        }
        //TODO
        for index in (0..self.characters.len()).rev() {
            if !self.characters[index].get_checked().is_alive(){
                self.characters.remove(index);
            }
        }
        for index in (0..self.monsters.len()).rev() {
            if !self.monsters[index].get_checked().is_alive(){
                self.monsters.remove(index);
            }
        }
        if self.characters.len() == 0{
            self.destroy();
            return;
        }
        if self.monsters.len() == 0{
            self.destroy();
            return;
        }
    }
    #[inline]
    pub fn is_valid(&self) -> bool{
        self.valid
    }

    pub(crate) fn add_character(&mut self, characters: Vec<MutexArc<Character>>){
        for character in characters {
            let uuid = character.get().get_state().uuid;
            if self.characters.iter().find(|m| m.get().data.get_state().uuid == uuid).is_none(){
                character.mut_checked().enter_combat();
                self.characters.push(character);
            }
        }
    }

    pub(crate) fn add_creature(&mut self, monsters: Vec<MutexArc<Creature>>) {
        for monster in monsters {
            let uuid = monster.get().get_state().uuid;
            if self.monsters.iter().find(|m| m.get().data.get_state().uuid == uuid).is_none(){
                monster.mut_checked().enter_combat();
                self.monsters.push(monster);
            }
        }
    }
}