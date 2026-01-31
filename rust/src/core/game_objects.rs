use crate::call_cpp;
use crate::common::*;
use crate::core::transform::Transform;
use crate::repositories::game_objects::*;

#[derive(Clone, Debug)]
struct GameObjectTransform {}
impl Transform for GameObjectTransform {
    fn get_position(&self) -> Vec3 {
        call_cpp!(CPP_GET_POSITION)
    }

    // fn get_rotation() -> Vec3;
    // fn get_scale() -> Vec3;

    // fn positionate() -> Vec3;
    // fn translate() -> Vec3;

    // fn rotate() -> Vec3;
    // fn set_angle() -> Vec3;

    // fn scalate() -> Vec3;
}

#[derive(Clone, Debug)]
struct GameObject {
    transform: GameObjectTransform,
}
