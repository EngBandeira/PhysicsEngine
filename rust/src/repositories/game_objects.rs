use crate::common::*;

pub type get_position = extern "C" fn() -> Vec3;
pub static mut CPP_GET_POSITION: Option<get_position> = None;
