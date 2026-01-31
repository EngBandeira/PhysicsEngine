mod common;
mod core;
mod repositories;
mod user;

use libc;
use std::{
    ffi::CStr,
    os::raw::{c_char, c_int, c_uint},
};

use crate::common::Callback;

#[macro_export]
macro_rules! call_cpp {
    ($function_path:path) => {
        match unsafe { $function_path } {
            Some(function) => function(),
            None => panic!("Function not found: {}", stringify!($function_path)),
        }
    };
}

#[macro_export]
macro_rules! generate_callback_index {
    ($function_path:path) => {
        match unsafe { $function_path } {
            Some(function) => function(),
            None => panic!("Function not found: {}", stringify!($function_path)),
        }
    };
}

#[unsafe(no_mangle)]
pub extern "C" fn export_cpp_functions(get_position: repositories::game_objects::get_position) {
    unsafe {
        repositories::game_objects::CPP_GET_POSITION = Some(get_position);
    }
}

#[unsafe(no_mangle)]
pub extern "C" fn get_rust_callbacks(amount: c_uint) -> *mut *mut common::Callback {
    let count = amount as usize * 2;
    let size = count * std::mem::size_of::<*mut common::Callback>();
    let array_ptr = unsafe { libc::malloc(size) } as *mut *mut common::Callback;

    if array_ptr.is_null() {
        return std::ptr::null_mut();
    }

    user::get_rust_callbacks(array_ptr);
    array_ptr
}
